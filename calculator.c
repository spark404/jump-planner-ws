/* $Id: calculator.c,v 1.1 2008/07/13 21:13:41 spark Exp $
 *
 *  Implementation of A* algorithm
 *  used to find the shortes jump route between
 *  systems
 */

#include <stdio.h>
#include <stdlib.h>

#include "dbaccess.h"
#include "calculator.h"


routeelement_t *calculator(long from, long to, float maxdistance) {

  if (from == to) 
    return NULL;  /* useless query */

  dbcontext_t *dbcontext;
  dbcontext = dbaccess_init();

  if (dbcontext == NULL) 
    return NULL;

  jumppoints_t *neighbours;
  neighbours = dbaccess_findnextjump(dbcontext, from, to, maxdistance);

  dbaccess_finish(dbcontext);

  return NULL;
}
