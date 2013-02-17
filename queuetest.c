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
 */

/* test program */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "prioqueue.h"
#include "dbaccess.h"
#include "calculator.h"
#include "btree.h"

int main(int argc, char *argv[]) {
  prioqueue_t *pqueue = prioqueue_init();
  btree_t *btree = btree_init();
  /*
  srandom(time(NULL));
  
  prioqueue_print((const prioqueue_t *) pqueue);
  
  int i;
  
  for (i=0; i<10; i++) {
    float v = (float)random()/1000;
    fprintf (stdout, "random value: %f\n", v);
    btree_addnode(btree, NULL, v);
    prioqueue_push (&pqueue, NULL, v);
  }
  int koekje=10;
  btree_addnode(btree, &koekje, 300000);

  int *snoep = btree_searchnode(btree, 300000);
  fprintf (stdout, "value = %d\n", *snoep);

  btree_searchnode(btree, -1);

  btree_destroy(btree, 0);


  prioqueue_print((const prioqueue_t *) pqueue);

  int j;
  for (i=0; i<10; i++) {
    jumproute_t *t = prioqueue_pop(&pqueue);
    fprintf (stdout, "Popped\n");
    for (j=0; j<10; j++) {
      float v = (float)random()/1000;
      fprintf (stdout, "random value: %f\n", v);
      prioqueue_push (&pqueue, NULL, v);
    }
    prioqueue_print((const prioqueue_t *) pqueue);
  }
  
  */
  jumproute_t *finalroute = NULL;

  //finalroute = calculator(30000969, 30003609, 10);
  finalroute = calculator(30001022, 30000382, 5);

  if (finalroute == NULL) {
    printf ("No solution found\n");
  }
  else {
    
    printf ("Route: (%d jumps)\n", finalroute->size);
    
    int jump;
    for (jump=1; jump < finalroute->size; jump++) {
      printf (" %8ld --- %2.2f ly --> %8ld\n",
	      finalroute->jumps[jump-1].solarsystemid,
	      finalroute->jumps[jump].distance,
	      finalroute->jumps[jump].solarsystemid);
    }
  }
}
