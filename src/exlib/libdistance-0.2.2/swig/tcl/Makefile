# $Id: Makefile,v 1.2 2004/11/30 00:26:59 jose Exp $

all: distance.so

distance_wrap.o: ../distance.i
	swig -tcl -namespace -module distance -o distance_wrap.c \
	    -v ../distance.i
	gcc -c -I../.. -I/usr/local/include/tcl8.4 -fpic distance_wrap.c

distance.so: distance_wrap.o
	gcc -shared  distance_wrap.o ../../libdistance.a \
	    -o distance.so

clean:
	rm -f *.o distance_wrap.c *.so *.core *_wrap.c distance.py
