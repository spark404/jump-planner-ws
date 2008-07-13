/* $Id: planner_ws.c,v 1.1 2008/07/13 21:13:41 spark Exp $
 * 
 * soap server
 */

#include "soapH.h"
#include "planner.nsmap"
#include <pthread.h>

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

  SOAP_SOCKET master_sock, sock;
  int i;

  soap_init(&soap);

  master_sock = soap_bind(&soap, NULL, port, BACKLOG);
  if(!soap_valid_socket(master_sock)) {
    fprintf (stderr, "Socket create failed, die die die\n");
    exit(1);
  }
  fprintf (stderr, "Socket created on port %d\n", port);

  pthread_mutex_init(&queue_cs, NULL);
  pthread_cond_init(&queue_cv, NULL);

  for (i=0; i < MAX_THR; i++) {
    soap_thr[i] = soap_copy(&soap);
    pthread_create(&tid[i], NULL, 
		   (void*)process_queue, 
		   soap_thr[i]);
    fprintf (stderr, "Thread %d created\n", i);

  }
  pthread_create(&monitorid, NULL,
		 (void*)soapserver_monitor,
		 NULL);
}

void *soapserver_monitor (void *data) {
  int s;
  for (;;) {
    fprintf (stderr, "waiting for accept\n");
    s = soap_accept(&soap);
    if (!soap_valid_socket(s)) {
      if (soap.errnum) {
	soap_print_fault(&soap, stderr);
	continue;
      }
      else {
	fprintf(stderr, "Server timed out\n");
	break;
      }
    }
    while (enqueue(s) == SOAP_EOM)
      sleep (1); // queue is full, wait for a bit
  }
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
      fprintf(stderr, "Waiting for thread %d to terminate... ", i);
      pthread_join(tid[i], NULL);
      fprintf(stderr, "terminated\n");
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
      fprintf(stderr, "served\n");
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
  route->__size=2;
  route->__ptr = (struct Jump *)malloc(sizeof(struct Jump) * route->__size);
 
  struct Jump *jumps = route->__ptr;
  jumps[0].system = 300096;
  jumps[0].distance = 0;
  jumps[1].system = 3000112;
  jumps[1].distance = 11.2;

  fprintf(stderr, "Something managed to call me with (%ld, %ld, %f)\n", from, to, dist);
  int i;
  for (i=0; i < route->__size ; i++) {
    fprintf(stderr, "item %d : %ld %f\n", i, jumps[i].system, jumps[i].distance);
  }

  routeelement_t *waypoints;
  waypoints = calculator(from, to, dist);

  if (waypoints == NULL) {
    return soap_sender_fault(soap, "Route calculation failed" , "calculator module returned empty result");
  }

  return SOAP_OK;
}
