# $Id: Makefile,v 1.2 2008/07/13 21:31:32 spark Exp $
#
# Makefile
#
 
SOAP_LIB=/usr/lib/libgsoap.a

CPPFLAGS=
LDFLAGS=-lpthread -lpq

SOURCES=planner_ws.c main.c soapServer.c soapC.c dbaccess.c calculator.c
OBJS=$(patsubst %.c,%.o,${SOURCES})

SOAPCPP2=/usr/bin/soapcpp2

planner: soapH.h ${OBJS}
	${CC} ${CPPFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${SOAP_LIB}

soapH.h soapServer.c soapC.c: planner_ws.h
	${SOAPCPP2} -L -S -c -x $<

clean:
	rm -f ${OBJS} soapH.h planner \
	soapStub.h soapC.c soapServer.c planner.wsdl planner.nsmap ns.xsd

