# $Id: Makefile,v 1.5 2004/11/29 21:42:14 jose Exp $

LIB=		distance
SRCS=		levenshtein.c hamming.c bloom.c needleman_wunsch.c jaccard.c
SRCS+=		minkowski.c damerau.c
MAN=		distance.3
CFLAGS+=	-g -Wall -Wunused
LDADD+=		-g

SUBDIR+=	test swig

CLEANFILES+=	distance.cat3

.include <bsd.lib.mk>
