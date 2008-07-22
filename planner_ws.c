/* $Id: planner_ws.c,v 1.3 2008/07/22 11:34:47 cvsd Exp $
 * 
 * soap server
 */

#include "soapH.h"
#include "planner.nsmap"
#include <pthread.h>
#include <syslog.h>

#include "prioqueue.h"
#include "calculator.h"

#define BACKLOG 100
#define MAX_THR 10
#define MAX_QUEUE 100

SOAP_SOCKET queue[MAX_QUEUE]; // The global request queue of sockets
int head = 0, tail = 0; // Queue head and tail

void *soapserver_monitor(void *);
void *process_queue(void*);
int enqueue(SOAP_SOCKET);
SOAP_SOCKET dequeue();


pthread_mutex_t queue_cs;
pthread_cond_t queue_cv; 
pthread_t tid[MAX_THR];
pthread_t monitorid;
struct soap soap; // global soap context for monitor
struct soap *soap_thr[MAX_THR];

void soapserver_start (int port) {

  SOAP_SOCKET master_sock;
  int i;

  soap_init(&soap);

  master_sock = soap_bind(&soap, NULL, port, BACKLOG);
  if(!soap_valid_socket(master_sock)) {
    syslog (LOG_CRIT, "Socket create failed, die die die\n");
    exit(1);
  }
#if defined(DEBUG)
  syslog (LOG_DEBUG, "Socket created on port %d\n", port);
#endif

  pthread_mutex_init(&queue_cs, NULL);
  pthread_cond_init(&queue_cv, NULL);

  for (i=0; i < MAX_THR; i++) {
    soap_thr[i] = soap_copy(&soap);
    pthread_create(&tid[i], NULL, 
		   (void*)process_queue, 
		   soap_thr[i]);
#if defined (DEBUG)
    syslog (LOG_DEBUG, "Thread %d created\n", i);
#endif

  }
  pthread_create(&monitorid, NULL,
		 (void*)soapserver_monitor,
		 NULL);
}

void *soapserver_monitor (void *data) {
  int s;
  for (;;) {
#if defined (DEBUG)
    syslog (LOG_DEBUG, "waiting for accept\n");
#endif
    s = soap_accept(&soap);
    if (!soap_valid_socket(s)) {
      if (soap.errnum) {
	soap_print_fault(&soap, stderr);
	continue;
      }
      else {
	syslog(LOG_NOTICE, "Server timed out\n");
	break;
      }
    }
    while (enqueue(s) == SOAP_EOM)
      sleep (1); // queue is full, wait for a bit
  }
  return NULL;
}

void soapserver_stop () {
  int i;
  for (i = 0; i < MAX_THR; i++)
    {
      while (enqueue(SOAP_INVALID_SOCKET) == SOAP_EOM)
	sleep(1);
    }
  for (i = 0; i < MAX_THR; i++)
    {
      syslog (LOG_NOTICE, "Waiting for thread %d to terminate... ", i);
      pthread_join(tid[i], NULL);
      syslog (LOG_NOTICE, "Thread %d terminated\n", i);
      soap_done(soap_thr[i]);
      free(soap_thr[i]);
    }
  pthread_mutex_destroy(&queue_cs);
  pthread_cond_destroy(&queue_cv);

  soap_done(&soap);
  return;
} 

void *process_queue(void *soap)
{
  struct soap *tsoap = (struct soap*)soap;
  for (;;)
    {
      tsoap->socket = dequeue();
      if (!soap_valid_socket(tsoap->socket))
	break;
      soap_serve(tsoap);
      soap_destroy(tsoap);
      soap_end(tsoap);
#if defined (DEBUG)
      syslog (LOG_DEBUG, "served\n");
#endif
    }
  return NULL;
}

int enqueue(SOAP_SOCKET sock)
{
  int status = SOAP_OK;
  int next;
  pthread_mutex_lock(&queue_cs);
  next = tail + 1;
  if (next >= MAX_QUEUE)
    next = 0;
  if (next == head)
    status = SOAP_EOM;
  else
    {
      queue[tail] = sock;
      tail = next;
    }
  pthread_cond_signal(&queue_cv);
  pthread_mutex_unlock(&queue_cs);
  return status;
}

SOAP_SOCKET dequeue()
{
  SOAP_SOCKET sock;
  pthread_mutex_lock(&queue_cs);
  while (head == tail)       pthread_cond_wait(&queue_cv, &queue_cs);
  sock = queue[head++];
  if (head >= MAX_QUEUE)
    head = 0;
  pthread_mutex_unlock(&queue_cs);
  return sock;
} 


/**
 * implementation of the soapfunction
 */
int ns__planner(struct soap *soap, long from, long to, double dist, struct JumpRoute *route) {

  syslog (LOG_INFO, "Planner request with (%ld, %ld, %f)\n", from, to, dist);

  jumproute_t *waypoints;
  waypoints = calculator(from, to, dist);

  if (waypoints == NULL) {
    return soap_sender_fault(soap, "Route calculation failed" , "calculator module returned empty result");
  }
  
  route->__ptr = (struct Jump *)malloc(sizeof(struct Jump) * waypoints->size);
  route->__size=waypoints->size;
 
  struct Jump *jumps = route->__ptr;

  int i;
  for (i=0; i < waypoints->size ; i++) {
#if defined (DEBUG)
    syslog (LOG_DEBUG, "item %d : %ld %f\n", i, 
	    waypoints->jumps[i].solarsystemid, 
	    waypoints->jumps[i].distance);
#endif
    jumps[i].system = waypoints->jumps[i].solarsystemid;
    jumps[i].distance = waypoints->jumps[i].distance;
  }

  return SOAP_OK;
}
