# Makefile of printer driver
# Verona, August 2007 -- Massimo Del Fedele
# Adapted from Samsung ML-85P driver by Rildo Pragana

CFLAGS ?= -O3 -Wall -std=gnu90

TARGETS=lbp800 parcapt

HEADERS=\
    machine.h \
    errcapt.h \
    bmcapt.h \
    lbp800.h \
    pport.h \
    timcapt.h \
    llcapt.h \
    pkcapt.h \
    hlcapt.h \
    parcapt.h \


.PHONY: all
all: $(TARGETS)

lbp800: errcapt.o bmcapt.o lbp800.o

parcapt: errcapt.o pport.o timcapt.o llcapt.o pkcapt.o hlcapt.o parcapt.o

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: install
install: all
	install -m a=rx lbp800 /usr/bin
	install -m a=rx lbp800-print /usr/bin
	install -m a=r ppd/Canon-LBP-800.ppd /usr/share/ppd/custom
	install -m a=rx foo2lbp800-wrapper /usr/bin
	install -m a=rx parcapt /usr/lib/cups/backend

.PHONY: uninstall
uninstall:
	rm -f -- /usr/bin/lbp800
	rm -f -- /usr/bin/lbp800-print
	rm -f -- /usr/share/ppd/custom/Canon-LBP-800.ppd
	rm -f -- /usr/bin/foo2lbp800-wrapper
	rm -f -- /usr/lib/cups/backend/parcapt

.PHONY: clean
clean:
	rm -f -- $(TARGETS) *.o
