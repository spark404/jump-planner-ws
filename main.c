/* $Id: main.c,v 1.2 2008/07/22 11:34:47 cvsd Exp $
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
#include <syslog.h>

#include "dbaccess.h"


/* Globals
 */
int keeprunning = 1; /* terminates main loop when set to 0 */

/* Commandline options
 */
struct opts 
{
  int foreground;
  int port; /* port for the soap listener */
};

/* Handler for signals
 */
void signal_handler(int signal) 
{
  syslog(LOG_NOTICE,"Signal %d received\n", signal);
  keeprunning = 0;
}

extern void soapserver_start (int port);
extern void soapserver_stop();

int main (int argc, char *argv[]) 
{
  char option;
  struct opts opts; /* commandline options */
  FILE *pidfile;
  struct sigaction myhandler;
  int result;

  /* default */
  opts.foreground = 0;
  opts.port = 1500;

  /* check options */
  while ((option = getopt (argc, argv, "fp:")) != EOF)
  {
    switch (option)
      {
      case 'f':
	opts.foreground = 1;
	break;
      case 'p':
	opts.port = atoi(optarg);
	break;
      }
    }

  /* initialize syslog */
#if defined (DEBUG)
  openlog("planner_ws", LOG_PID | LOG_PERROR, LOG_LOCAL2);
#else
  openlog("planner_ws", LOG_PID, LOG_LOCAL2);
#endif

  /* set signal handler for KILL,HUP,TERM */
  memset(&myhandler, 0, sizeof (myhandler));
  myhandler.sa_handler = signal_handler;

  sigaction (SIGINT, &myhandler, NULL);
  sigaction (SIGHUP, &myhandler, NULL);
  sigaction (SIGTERM, &myhandler, NULL);
  
  if (! opts.foreground) {
    int fd, fdnum;

    pidfile = fopen ("/var/run/planner-service.pid","w");
    if (pidfile == NULL) {
      syslog (LOG_CRIT, "Unable to write pidfile: %s", strerror(errno));
      exit (1);
    }

    result = fork();
    if (result < 0) {
      perror("fork");
      exit(1);
    }
    else if (result) {
      /* Parent */
      syslog (LOG_NOTICE, "Pid is %d\n", result);
      fprintf (pidfile, "%d\n", result);
      fclose (pidfile);
      exit(0);
    }

    /* Child */
    fclose(pidfile);
    if (setsid() == -1) {
      syslog(LOG_WARNING,"bad setsid - %s\n",strerror(errno));
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
  while (keeprunning) {
    sleep (10);
  }

  // dbthreads_start(15);
  soapserver_stop();
  closelog();
  unlink("/var/run/planner-service.pid");
  exit (0);
}
