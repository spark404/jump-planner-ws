# $Id: Makefile,v 1.9 2008/07/22 11:34:47 cvsd Exp $
#
# Makefile


SOAP_LIB=/usr/lib/libgsoap.a

CPPFLAGS=-O3 -march=i686 -fomit-frame-pointer -pipe
#CPPFLAGS=
#CPPFLAGS=-pg
#CPPFLAGS= -g -DDEBUG -Wall
LDFLAGS=-lpthread -lpq

SOURCES=planner_ws.c main.c soapServer.c soapC.c dbaccess.c calculator.c prioqueue.c btree.c
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
