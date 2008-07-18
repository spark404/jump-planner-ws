/* $Id: dbaccess.c,v 1.2 2008/07/18 07:16:00 cvsd Exp $
 *
 * Database access routines for the planner
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "libpq-fe.h"
#include "dbaccess.h"

#define DBACCESS_OK 0
#define DBACCESS_FAIL 1

/** Internal functions
 */
int dbaccess_preparestatements(dbcontext_t *dbcontext);

dbcontext_t *dbaccess_init() {
  if (!PQisthreadsafe()) {
    fprintf (stderr, "The version/compilation of libpq is not threadsafe!\n");
    return NULL;
  }
  dbcontext_t *dbcontext;
  dbcontext = (dbcontext_t *)malloc(sizeof(dbcontext_t));

  dbcontext->connection = PQsetdbLogin("localhost",
				       NULL,
				       NULL,
				       NULL,
				       "evedb-trinity",
				       "planner_ws",
				       NULL);
  if (PQstatus(dbcontext->connection) != CONNECTION_OK) {
    fprintf (stderr, "Connection to the database failed.\n");
    PQfinish(dbcontext->connection);
    free(dbcontext);
    return NULL;
  }
  
  if (dbaccess_preparestatements(dbcontext) != DBACCESS_OK) {
    fprintf (stderr, "Statement preparation failed\n");
    PQfinish(dbcontext->connection);
    free(dbcontext);
    return NULL;
  }

  return dbcontext;
}

void dbaccess_finish(dbcontext_t *dbcontext) {
  PGconn *conn = dbcontext->connection;

  /* should we remove the prepared statements? */
  PQfinish(conn);

  free (dbcontext);
}

int dbaccess_preparestatements(dbcontext_t *dbcontext) {
  PGconn *conn = dbcontext->connection;
  PGresult *result; 

  /* find distance between two nodes */
  result = PQprepare (conn,
		      "findDistance",
		      "SELECT distance \
                       FROM eve.jumpdistances \
                       WHERE from_solarsystemid=$1 \
                         AND to_solarsystemid=$2",
		      2,
		      NULL); // Server will figure out the datatypes

  if (PQresultStatus(result) != PGRES_COMMAND_OK) {
    fprintf(stderr, "Failed to create statement 'findDistance': %s", PQerrorMessage(conn));
    PQclear(result);
    return DBACCESS_FAIL;
  }

  /* find neighbours based on max distance */
  result = PQprepare (conn,
		      "findNeighbours",
		      "SELECT toss.solarsystemid AS to_solarsystemid, \
                              sqrt((toss.x - fromss.x)^2 + (toss.y - fromss.y)^2 + (toss.z - fromss.z)^2) / 946e+13::bigint::numeric::double precision AS distance, \
                              sqrt((toss.x - endss.x)^2 + (toss.y - endss.y)^2 + (toss.z - endss.z)^2) / 946e+13::bigint::numeric::double precision AS remaining_distance \
                       FROM dbo.mapsolarsystems fromss \
                       JOIN dbo.mapsolarsystems toss \
                          ON fromss.solarsystemid <> toss.solarsystemid \
                          AND sqrt((toss.x - fromss.x)^2 + (toss.y - fromss.y)^2 + (toss.z - fromss.z)^2) / 946e+13::bigint::numeric::double precision < $1 \
                          AND fromss.solarsystemid=$2 \
                          AND toss.\"security\" < $3 \
                       JOIN dbo.mapsolarsystems endss \
                          ON endss.solarsystemid=$4 \
                        ORDER BY remaining_distance",
		      4,
		      NULL); // Server will figure out the datatypes

  if (PQresultStatus(result) != PGRES_COMMAND_OK) {
    fprintf(stderr, "Failed to create statement 'findNeighbours': %s", PQerrorMessage(conn));
    PQclear(result);
    return DBACCESS_FAIL;
  }

  return DBACCESS_OK;
}

/**
 *  dbcontext   : the current db context with connection id
 *  from        : the current solarsystemid
 *  to          : the final destination
 *  maxdistance : the maximum jumpdistance
 *
 *  returns a linked list with jumppoints, ordered by lowest remaining distance
 */
jumppoints_t *dbaccess_findnextjump(dbcontext_t *dbcontext, long from, long to, float maxdistance) {
  PGconn     *conn;
  PGresult   *result;
  char       *paramValues[4];
  int        paramLengths[4];
  int        paramFormats[4];
  jumppoints_t *jumpoptions = NULL;

  /* 0: max distance */
  paramValues[0] = (char *)malloc(17);
  snprintf (paramValues[0], 17, "%16lf", maxdistance);
  paramLengths[0] = strlen(paramValues[0]);
  paramFormats[0] = 0; /* text representation */

  /* 1: from solarsystem */
  paramValues[1] = (char *)malloc(17);
  snprintf (paramValues[1], 17, "%16ld", from);
  paramLengths[1] = strlen(paramValues[1]);
  paramFormats[1] = 0; /* text representation */

  /* 2: security rating */
  paramValues[2] = "0.45"; // Fixed setting for now
  paramLengths[2] = strlen(paramValues[2]);
  paramFormats[2] = 0; /* text representation */

  /* 3: from solarsystem */
  paramValues[3] = (char *)malloc(17);
  snprintf (paramValues[3], 17, "%16ld", to);
  paramLengths[3] = strlen(paramValues[0]);
  paramFormats[3] = 0; /* text representation */
  
#if defined (DEBUG)
  int i;
  fprintf(stderr, "Preparing to execute  query 'findNeighbours':\n");
  for (i=0; i< 4; i++) {
    fprintf(stderr, " Param %d : '%s' (len=%d)\n", i, paramValues[i], paramLengths[i]);
  }
#endif

  conn = dbcontext->connection;
  
  result = PQexecPrepared(conn,
	               "findNeighbours",
		       4,
		       (const char *const*)paramValues,
		       paramLengths,
		       paramFormats,
		       0); /* returnFormat = 0, return in text format */

  if (PQresultStatus(result) != PGRES_TUPLES_OK) {
    fprintf(stderr, "Failed to execute prepared query 'findNeighbours': (%d) %s", PQresultStatus(result), PQerrorMessage(conn));
    PQclear(result);
    return NULL;
  }

#if defined(DEBUG)
  fprintf (stderr, "Query succeeded with %d rows in the result\n", PQntuples(result));
#endif

  int row;
  for (row=0; row < PQntuples(result); row++ ) {
    jumppoints_t *current;
    jumppoints_t *temp = (jumppoints_t *)malloc(sizeof(jumppoints_t));

    temp->solarsystemid = strtol(PQgetvalue(result, row, 0), NULL, 10);
    temp->distance = (float)strtod(PQgetvalue(result, row, 1), NULL) ;
    temp->remaining_distance = (float)strtod(PQgetvalue(result, row, 2), NULL);
    temp->next = NULL;

    /* your run-of-the-mill linked list */
    if (row == 0) {
      current = temp;
      jumpoptions = current;
    }
    else {
      current->next = temp;
      current = current->next;
    }

#if defined(DEBUG)
    /* DEBUG */
    fprintf (stderr, " %3d> %s | %s | %s\n", row, 
                                             PQgetvalue(result, row, 0),
                                             PQgetvalue(result, row, 1),
                                             PQgetvalue(result, row, 2));
#endif  

  }

  PQclear(result);
  free(paramValues[0]);
  free(paramValues[1]);
  //free(paramValues[2]);
  free(paramValues[3]);
  return jumpoptions;
}

/**
 *  dbcontext   : the current db context with connection id
 *  from        : the current solarsystemid
 *  to          : the final destination
 *
 *  returns a float with the distance between the two nodes
 */
float dbaccess_finddistance(dbcontext_t *dbcontext, long from, long to) {
  PGconn     *conn;
  PGresult   *result;
  char       *paramValues[2];
  int        paramLengths[2];
  int        paramFormats[2];

  /* 0: from solarsystem */
  paramValues[0] = (char *)malloc(17);
  snprintf (paramValues[0], 17, "%16ld", from);
  paramLengths[0] = strlen(paramValues[0]);
  paramFormats[0] = 0; /* text representation */

  /* 1: to solarsystem */
  paramValues[1] = (char *)malloc(17);
  snprintf (paramValues[1], 17, "%16ld", to);
  paramLengths[1] = strlen(paramValues[1]);
  paramFormats[1] = 0; /* text representation */
  
#if defined(DEBUG)
  int i;
  fprintf(stderr, "Preparing to execute  query 'findDistance':\n");
  for (i=0; i< 2; i++) {
    fprintf(stderr, " Param %d : '%s' (len=%d)\n", i, paramValues[i], paramLengths[i]);
  }
#endif

  conn = dbcontext->connection;
  
  result = PQexecPrepared(conn,
	               "findDistance",
		       2,
		       (const char *const*)paramValues,
		       paramLengths,
		       paramFormats,
		       0); /* returnFormat = 0, return in text format */

  if (PQresultStatus(result) != PGRES_TUPLES_OK) {
    fprintf(stderr, "Failed to execute prepared query 'findDistance': (%d) %s", PQresultStatus(result), PQerrorMessage(conn));
    PQclear(result);
    return -1;
  }

#if defined(DEBUG)
  fprintf (stderr, "Query succeeded with %d rows in the result\n", PQntuples(result));
  int j;
  for (j=0 ; j< PQntuples(result); j++) {
    fprintf (stderr, " %3d> %s \n", j, 
	     PQgetvalue(result, j, 0));
  }
#endif

  if (PQntuples(result) != 1) {
    fprintf(stderr, "Strange result from database for distance between %ld and %ld\n", from, to);
    return -1;
  }

  float dist = (float)strtod(PQgetvalue(result, 0, 0), NULL);
#if defined(DEBUG)
  fprintf(stderr, "string %s converted to %f\n", PQgetvalue(result, 0, 0), dist);
#endif
  PQclear(result);
  free(paramValues[0]);
  free(paramValues[1]);
  return dist;
}
