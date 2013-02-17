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

