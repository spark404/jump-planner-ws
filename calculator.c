/**
 * Copyright 2008 Hugo Trippaers <opensource@strocamp.net>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  Implementation of A* algorithm used to find the shortes jump route 
 *  between systems.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbaccess.h"
#include "prioqueue.h"
#include "calculator.h"
#include "btree.h"

/* internal function */
jumproute_t *extend_jumproute(jumproute_t *workroute, 
			      jumppoints_t *neighbour);

jumproute_t *calculator(long from, long to, float maxdistance) {

  if (from == to) 
    return NULL;  /* useless query */

  dbcontext_t *dbcontext;
  dbcontext = dbaccess_init();

  if (dbcontext == NULL) 
    return NULL;

  prioqueue_t *pqueue = prioqueue_init();
  btree_t *closed = btree_init();
  int btree_data = 1;

  jumprouteelement_t startnode;
  startnode.solarsystemid=from;
  startnode.distance=dbaccess_finddistance(dbcontext, from, to);

  jumproute_t *startroute = (jumproute_t *)malloc(sizeof(startroute));
  startroute->size=0;
  startroute->jumps[startroute->size++] = startnode;

  prioqueue_push(&pqueue, startroute, startroute->jumps[0].distance);

#if defined(DEBUG)
  fprintf(stderr, "Starting with node %ld with a distance of %f to %ld\n",
	  from, startroute->jumps[0].distance, to);
#endif

  jumproute_t *finalroute;
  jumproute_t *workroute;
  while ((workroute = prioqueue_pop(&pqueue)) != NULL) {
#if defined(DEBUG)
    int j;
    for (j=0; j < workroute->size; j++) {
      fprintf(stderr, "jump %ld %f ly\n", 
	      workroute->jumps[j].solarsystemid,
	      workroute->jumps[j].distance);
    }
#endif
    if (workroute->jumps[workroute->size-1].solarsystemid == to) {
      finalroute = workroute;
      break;
    }
    
    long currentnode = workroute->jumps[workroute->size-1].solarsystemid;
    if (btree_searchnode(closed, (double)currentnode) == NULL) {
    btree_addnode(closed, &btree_data, (double)currentnode);
      
      jumppoints_t *neighbours;
      neighbours = dbaccess_findnextjump(dbcontext, 
					 currentnode, 
					 to, 
					 maxdistance);
      
      jumppoints_t *neighbour = neighbours;
      while (neighbour != NULL) {
	/* no need to add neighbour if it is closed */
	if (btree_searchnode(closed, (double)neighbour->solarsystemid) == NULL) {
	  /* clone workroute and extend with current neighbour */
	  jumproute_t *newroute = extend_jumproute(workroute, neighbour);
	  prioqueue_push(&pqueue, newroute, neighbour->remaining_distance);
#if defined (DDEBUG)
	  fprintf(stderr, "queue count = %d\n", prioqueue_count(pqueue));
#endif
	}
	jumppoints_t *old = neighbour;
	neighbour = neighbour->next;
	free (old);
      }
    }
#if defined (DDEBUG)
    prioqueue_print(pqueue);
#endif
    free(workroute);
  }

  dbaccess_finish(dbcontext);

  return finalroute;
}


jumproute_t *extend_jumproute(jumproute_t *workroute, 
			      jumppoints_t *neighbour) {
  jumproute_t *clone = (jumproute_t *)malloc(sizeof(jumproute_t));
  memcpy(clone, workroute, sizeof(jumproute_t));
  clone->jumps[clone->size].solarsystemid = neighbour->solarsystemid; 
  clone->jumps[clone->size].distance = neighbour->distance;
  clone->size++;

  return clone;
}
