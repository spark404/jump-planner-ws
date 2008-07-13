/* $Id: planner_ws.h,v 1.1 2008/07/13 21:13:41 spark Exp $
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

