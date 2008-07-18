/* $Id: prioqueue.c,v 1.1 2008/07/18 07:13:57 cvsd Exp $
 *
 * priority queue module
 */

#include <stdio.h>
#include <stdlib.h>

#include "prioqueue.h"

prioqueue_t *prioqueue_init() {
  prioqueue_t *pqueue = NULL;

  return pqueue;
}

void prioqueue_push(prioqueue_t **pqueue, jumproute_t *jump, float priority) {
  /* linked list push
   * sorted by prio
   */

  /* create new element */
  prioqueueelement_t *newelement = 
    (prioqueueelement_t *)malloc(sizeof(prioqueueelement_t));
  newelement->priority=priority;
  newelement->jumproute = jump;
  
  /* create new prioqueue node */
  prioqueue_t *newitem = (prioqueue_t *)malloc(sizeof(prioqueue_t));
  newitem->pqe = newelement;
  newitem->next = NULL;
  
  /* find a spot in the queue */
  if ((*pqueue) == NULL ) {
    /* Empty list */
    (*pqueue) = newitem;

#if defined(DEBUG)
    fprintf(stderr, "Adding element to empty list\n");
#endif
  }
  else {
    prioqueue_t *current = (*pqueue);
    prioqueue_t *previous = NULL;

    while (current->next != NULL && 
           priority > current->pqe->priority ) {
      previous = current;
      current = current->next;
    }

    if (priority < current->pqe->priority ) {
      /* insert before */
      newitem->next = current;
      if (previous != NULL) 
	previous->next = newitem;
      else 
	*pqueue = newitem;
      
#if defined(DEBUG)
      fprintf(stderr, "Adding element before current\n");
#endif
    }
    else if (priority == current->pqe->priority) {
#if defined(DEBUG)
      fprintf(stderr, "ignoring equal priority\n");
#endif
      free(newelement);
      free(newitem);
      return;
    }
     else {
      /* insert after, must be at the end of the list */
      current->next = newitem;
      
#if defined(DEBUG)
      fprintf(stderr, "Adding element after last\n");
#endif
    }
  }
}


jumproute_t *prioqueue_pop(prioqueue_t **pqueue) {
  if ((*pqueue) == NULL)
    return NULL;
  jumproute_t *jr = (*pqueue)->pqe->jumproute;
  prioqueue_t *top = (*pqueue);
  if (top->next == NULL) {
    /* empty list after pop */
    (*pqueue) = NULL;
  }
  else {
    (*pqueue) = top->next;
    free(top->pqe);
    free(top);
  }

  return jr;
}

/* DEBUG */
void prioqueue_print (prioqueue_t *queue) {
  prioqueue_t *current;
  if (queue == NULL) {
    fprintf (stderr, "empty list\n");
  }
  
  int i = 0;
  for (current = queue; current != NULL; current = current->next) {
    fprintf (stderr, "item %4d has priority %4.3f\n", 
	     i++, current->pqe->priority );
  }
}

int prioqueue_count (prioqueue_t *queue) {
  prioqueue_t *current;
  if (queue == NULL) {
    return 0;
  }
  
  int i = 0;
  for (current = queue; current != NULL; current = current->next) {
    i++;
  }
  return i;
}
  
