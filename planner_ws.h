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
 *
 *  Planner Webservice description
 *
 *  This file needs to be processed by
 *  soapcpp2 to generate the server stub
 *  or the client stub.
 */

//gsoap ns service name:        planner Simple jumproute planner
//gsoap ns service style:       soap
//gsoap ns service encoding:    encoded
//gsoap ns service namespace:   http://ymir.strocamp.net/services.wsdl
//gsoap ns service location:    http://ymir.strocamp.net/services/planner

//gsoap ns schema namespace:    urn:planner

//gsoap ns service method-documentation: plan a route

typedef long xsd__long;
typedef double xsd__double;

typedef struct Jump {
	xsd__long system;
	xsd__double distance;
} ns__Jump;

typedef struct JumpRoute {
	ns__Jump *__ptr;
	int __size;
} ns__JumpRoute;

int ns__planner(long from, long to, double max_jumpdistance, ns__JumpRoute *route);

