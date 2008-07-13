/* $Id: calculator.h,v 1.1 2008/07/13 21:13:41 spark Exp $
 *
 *  Implementation of A* algorithm
 *  used to find the shortes jump route between 
 *  systems
 */

typedef struct prioqueue {
  void *item;
  float priority;
  struct prioqueue *next;
} prioqueue_t;

void prioqueue_push(prioqueue_t *queue, void *item, float priority);
void *prioqueue_pop(prioqueue_t *queue);

typedef struct routeelement {
  long solarsystemid;
  float distance;
  struct routeelement *next;
} routeelement_t;

void routeelement_push (routeelement_t *route, routeelement_t item);

/* the main calculator function */
routeelement_t *calculator (long from, long to, float max_distance);
  
