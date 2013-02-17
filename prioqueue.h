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
#define MAX_JUMPS 40

typedef struct jumprouteelement {
  long solarsystemid;
  float distance;
} jumprouteelement_t;

typedef struct jumproute {
  int size;  // amount of elements in the array
  jumprouteelement_t jumps[MAX_JUMPS];
} jumproute_t;

typedef struct prioqueueelement {
  jumproute_t *jumproute;
  float priority;  // priority of this item
} prioqueueelement_t;

/* linked list */
typedef struct prioqueue {
  prioqueueelement_t *pqe;
  struct prioqueue *next;
} prioqueue_t;

prioqueue_t *prioqueue_init();
void prioqueue_push(prioqueue_t **queue, jumproute_t *jump, float priority);
jumproute_t *prioqueue_pop(prioqueue_t **queue);
void prioqueue_print(prioqueue_t *queue);
int prioqueue_count(prioqueue_t *queue);
