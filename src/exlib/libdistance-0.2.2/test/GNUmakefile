# $Id: GNUmakefile,v 1.5 2004/05/20 21:52:01 jose Exp $
#
# GNU Makefile, works for mingw, should work for other gmake systems.

test:	test.c ../libdistance.a
	gcc -g -c -I.. test.c
	gcc -g -L.. -o test test.o -ldistance -lm

clean:
	rm -f *.core *.o test test.exe
