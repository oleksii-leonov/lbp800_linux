# Makefile of printer driver
# Verona, August 2007 -- Massimo Del Fedele
# Adapted from Samsung ML-85P driver by Rildo Pragana

CC=gcc
CFLAGS=-O2 -s -Wall
RELEASE=0.1.2

all: lbp800 parcapt

lbp800: \
    machine.h \
    errcapt.c errcapt.h \
    bmcapt.c bmcapt.h \
    lbp800.c lbp800.h \
    Makefile
	${CC} ${INCLUDE} ${CFLAGS} \
	errcapt.c bmcapt.c lbp800.c \
	-l cups -o bin/Release/lbp800

parcapt: \
    machine.h \
    errcapt.c errcapt.h \
    pport.c pport.h \
    timcapt.c timcapt.h \
    llcapt.c llcapt.h \
    pkcapt.c pkcapt.h \
    hlcapt.c hlcapt.h \
  	parcapt.h \
	  parcapt.c \
	  Makefile
	${CC} ${INCLUDE} ${CFLAGS} \
	errcapt.c pport.c timcapt.c llcapt.c pkcapt.c hlcapt.c parcapt.c \
	-l cups -o bin/Release/parcapt


install: all
	install -s -m a=rx bin/Release/lbp800 /usr/bin
	install -m a=rx lbp800-print /usr/bin
	install -m a=r ppd/Canon-LBP-800.ppd /usr/share/ppd/custom
	install -m a=rx foo2lbp800-wrapper /usr/bin
	install -m a=rx bin/Release/parcapt /usr/lib/cups/backend

uninstall:
	rm -f /usr/bin/lbp800
	rm -f /usr/bin/lbp800-print
	rm -f /usr/share/ppd/custom/Canon-LBP-800.ppd
	rm -f /usr/bin/foo2lbp800-wrapper
	rm -f /usr/lib/cups/backend/parcapt

clean:
	rm -f obj/Debug/*
	rm -f obj/Release/*
	rm -f bin/Release/lbp800 bin/Release/parcapt
	rm -f bin/Debug/lbp800 bin/Debug/parcapt

package: clean
	rm -f *~ ppd/*~
	cp -a ../lbp800 ../lbp800-${RELEASE}
	tar cvfz ../lbp800-${RELEASE}.tar.gz ../lbp800-${RELEASE}
	rm -rf ../lbp800-${RELEASE}
