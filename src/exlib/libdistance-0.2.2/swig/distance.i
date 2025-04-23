/*
	$Id: distance.i,v 1.3 2004/11/29 21:48:08 jose Exp $

	edit-distance is a library which uses various means to 
	calculate the "edit distance" between two strings. interfaces
	exist for the levenshtein distance and the hamming distance.

	this will provide the interface for swig (and thereby tcl and
	python).
 */

%module distance

%{
#include "distance.h"
%}

int 	levenshtein_d(const char *d1, size_t len1, const char *d2, size_t len2);
int 	damerau_d(const char *d1, size_t len1, const char *d2, size_t len2);
int 	hamming_d(const char *d1, size_t len1, const char *d2, size_t len2);
float 	jaccard_d(const char *d1, size_t len1, const char *d2, size_t len2);
void 	bloom_create(const void *data, size_t len, const void *digest, 
		     size_t digest_len);
double  bloom_d(const void *digest1, const void *digest2, size_t digest_len);
double  needleman_wunsch_d(const void *d1, size_t len1, const void *d2, 
		     size_t len2, struct matrix *m);
