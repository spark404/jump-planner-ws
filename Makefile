# Copyright 2008 Hugo Trippaers <opensource@strocamp.net>
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

SOAP_LIB=/usr/lib/libgsoap.a

CPPFLAGS=-O3 -march=i686 -fomit-frame-pointer -pipe
#CPPFLAGS=
#CPPFLAGS=-pg
#CPPFLAGS= -g -DDEBUG -Wall
LDFLAGS=-lpthread -lpq

SOURCES=planner_ws.c main.c soapServer.c soapC.c dbaccess.c calculator.c prioqueue.c btree.c redblack.c
OBJS=$(patsubst %.c,%.o,${SOURCES})

SOAPCPP2=/usr/bin/soapcpp2

planner: soapH.h ${OBJS}
	${CC} ${CPPFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${SOAP_LIB}

soapH.h soapServer.c soapC.c: planner_ws.h
	${SOAPCPP2} -L -S -c -x $<

clean:
	rm -f ${OBJS} queuetest.o soapH.h planner \
	soapStub.h soapC.c soapServer.c planner.wsdl planner.nsmap ns.xsd

queuetest: prioqueue.o queuetest.o calculator.o dbaccess.o btree.o
	${CC} ${CPPFLAGS} -lpq -o $@ prioqueue.o calculator.o queuetest.o dbaccess.o btree.o
