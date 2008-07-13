/* $Id: main.c,v 1.1 2008/07/13 21:13:41 spark Exp $
 *
 * Main program for the planner webservice
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "dbaccess.h"
#include <syslog.h>

/* Globals
 */
int keeprunning = 1; /* terminates main loop when set to 0 */

/* Commandline options
 */
struct opts 
{
  char fork;
  int port; /* port for the soap listener */
};

/* Handler for signals
 */
void signal_handler(int signal) 
{
  keeprunning = 0;
}


main (int argc, char *argv[]) 
{
  char option;
  struct opts opts; /* commandline options */
  FILE *pidfile;
  struct sigaction myhandler;
  int result;

  /* default */
  opts.fork = 1;
  opts.port = 1500;

  /* check options */
  while ((option = getopt (argc, argv, "fp:")) != EOF)
  {
    switch (option)
      {
      case 'f':
	opts.fork = 1;
	break;
      case 'p':
	opts.port = atoi(optarg);
	break;
      }
    }

  /* set signal handler for KILL,HUP,TERM */
  memset(&myhandler, 0, sizeof (myhandler));
  myhandler.sa_handler = signal_handler;

  sigaction (SIGHUP, &myhandler, NULL);
  sigaction (SIGTERM, &myhandler, NULL);
  
  if (! opts.fork) {
    int fd, fdnum;

    result = fork();
    if (result < 0) {
      perror("fork");
      exit(1);
    }
    else if (result) {
      printf ("Pid is %d\n", result);
      pidfile = fopen ("/var/run/planner-service.pid","w");
      fprintf (pidfile, "%d\n", result);
      fclose (pidfile);
      exit(0);
    }

    if (setsid() == -1) {
      fprintf(stderr,"bad setsid - %s\n",strerror(errno));
      exit(1);
    }

    fd=open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);

    fdnum = getdtablesize();
    for ( fd = (STDERR_FILENO + 1); fd < fdnum; fd++ )
      close(fd);
  }

  soapserver_start(opts.port);
  for (;;) {
    sleep (60);
  }
  // dbthreads_start(15);
  exit (0);
}
