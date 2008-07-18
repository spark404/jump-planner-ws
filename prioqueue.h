/* $Id: prioqueue.h,v 1.1 2008/07/18 07:13:57 cvsd Exp $
 *
 * priority queue module
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
