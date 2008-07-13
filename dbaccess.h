/* $Id: dbaccess.h,v 1.1 2008/07/13 21:13:41 spark Exp $ 
 * 
 * DB access routines
 * 
 * should be thread safe ?!?
 */

#include "libpq-fe.h"

typedef struct dbcontext {
  PGconn *connection;
} dbcontext_t;

typedef struct jumppoints {
  long solarsystemid;
  float distance;
  float remaining_distance;
  struct jumppoints *next;
} jumppoints_t;

dbcontext_t *dbaccess_init();
jumppoints_t *dbaccess_findnextjump(dbcontext_t *dbcontext, long from, long to, float maxdistance);
void dbaccess_finish(dbcontext_t *dbcontext);

