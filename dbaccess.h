/* $Id: dbaccess.h,v 1.2 2008/07/18 07:16:01 cvsd Exp $ 
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
float dbaccess_finddistance(dbcontext_t *dbcontext, long from, long to);
void dbaccess_finish(dbcontext_t *dbcontext);

