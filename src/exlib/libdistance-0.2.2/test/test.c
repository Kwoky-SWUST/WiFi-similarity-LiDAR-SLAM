/* $Id: test.c,v 1.8 2004/11/30 00:21:20 jose Exp $ */

/*
   libdistance unit tests

   test all of the public interfaces to libdistance.
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef __Darwin__
#include <stdint.h>
#endif				/* __Darwin__ */
#include <math.h>
#include "distance.h"

static int      num_tests = 0;
static int      num_pass = 0;

/* compare expected, received integers */

static void
test_int_result(int expected, int recvd)
{
	if (recvd == expected) {
		printf("PASS\n");
		num_pass++;
	} else
		printf("FAIL, expected %d, got %d\n", expected, recvd);

	num_tests++;

	return;
}

static void
test_double_result(double expected, double recvd)
{
	if (fabs(recvd - expected) < 0.0001) {
		printf("PASS\n");
		num_pass++;
	} else
		printf("FAIL, expected %f, got %f\n", expected, recvd);

	num_tests++;

	return;
}

static void
test_ld(void)
{
	int             l;

	/* LD inputs */
	char           *l1 = "Ge t gen:eric Via-gr@ f:or as low as $2.50 per 50 mg   bryce";
	char           *l2 = "G et generi:c Via-gra f(o)r as 1ow as $2.50 per 50 mg  southampton";
	char           *l3 = "G et generi:c Via-gra f(o)r as 1ow as $2.50 per 50 mg  southampton";
	char	       *l4 = "";
	char	       *l5 = "foo bar";
	char           *l6 = "foo   bar";

	printf("testing levenshtein_d()\n");

	l = levenshtein_d(l1, strlen(l1), l2, strlen(l2));
	printf("levenshtein_d is %d ", l);
	test_int_result(19, l);
	l = levenshtein_d(l2, strlen(l2), l3, strlen(l3));
	printf("levenshtein_d is %d ", l);
	test_int_result(0, l);
	l = levenshtein_d(l4, strlen(l4), l3, strlen(l3));
	printf("levenshtein_d is %d ", l);
	test_int_result(strlen(l3), l);
	l = levenshtein_d(l5, strlen(l5), l6, strlen(l6));
	printf("levenshtein_d is %d ", l);
	test_int_result(2, l);

	printf("strlen of s is %d, strlen of t is %d\n", strlen(l1), strlen(l2));
	
	return;
}

static void
test_dd(void)
{
	int d;

	char *d1 = "hello my name is jose nazario";
	char *d2 = "hlelo my name is jos enazario";
	char *d3 = "lehlo my name is jose nazario";
	char *d4 = "hello  my name is jose nazario";
	char *d5 = "hello  my name is josenazario";

	printf("testing daemerau_d()\n");

	d = damerau_d(d1, strlen(d1), d1, strlen(d1));
	printf("damerau_d is %d ", d);
	test_int_result(0, d);

	d = damerau_d(d1, strlen(d1), d2, strlen(d2));
	printf("damerau_d is %d ", d);
	test_int_result(0, d);

	d = damerau_d(d1, strlen(d1), d3, strlen(d3));
	printf("damerau_d is %d ", d);
	test_int_result(2, d);

	d = damerau_d(d1, strlen(d1), d4, strlen(d4));
	printf("damerau_d is %d ", d);
	test_int_result(1, d);

	d = damerau_d(d1, strlen(d1), d5, strlen(d5));
	printf("damerau_d is %d ", d);
	test_int_result(2, d);

	return;
}

static void
test_hd(void)
{
	int             h;

	/* HD inputs */
	char           *h1 = "Ge t gen:eric Via-gr@ f:or as low as $2.50 per 50 mg ";
	char           *h2 = "G et generi:c Via-gra f(o)r as 1ow as $2.50 per 50 mg";
	char           *h3 = "G et generi:c Via-gra f(o)r as 1ow as $2.50 per 50 mg";

	char           *l1 = "Ge t gen:eric Via-gr@ f:or as low as $2.50 per 50 mg   bryce";
	char           *l2 = "G et generi:c Via-gra f(o)r as 1ow as $2.50 per 50 mg  southampton";

	printf("testing hamming_d()\n");

	h = hamming_d(h1, strlen(h1), h2, strlen(h2));
	printf("hamming_d is %d ", h);
	test_int_result(36, h);
	h = hamming_d(h2, strlen(h2), h3, strlen(h3));
	printf("hamming_d is %d ", h);
	test_int_result(0, h);
	h = hamming_d(l1, strlen(l1), l2, strlen(l2));
	printf("hamming_d is %d ", h);
	test_int_result(-1, h);

	return;
}

static void
test_bloom(void)
{
	char           *d1, *d2, *d3;
	double          b;
	int             len, i;

	/* Bloom inputs */
	char           *s1 = "Ge t gen:eric Via-gr@ f:or as low as $2.50 per 50 mg   bryce";
	char           *s2 = "G et generi:c Via-gra f(o)r as 1ow as $2.50 per 50 mg  southampton";
	char           *s3 = "and now for something completely, totally, utterly different";

	printf("testing bloom_d()\n");

	//test 64 bit digests, create bloom filters
	len = 8;
	d1 = (char *) malloc(len);
	d2 = (char *) malloc(len);
	d3 = (char *) malloc(len);
	if (!d1 || !d2 || !d3)
		fprintf(stderr, "could not allocate digests");

	bloom_create(s1, strlen(s1), d1, len);
	bloom_create(s2, strlen(s2), d2, len);
	bloom_create(s3, strlen(s3), d3, len);

	b = bloom_d(d1, d1, len);
	printf("bloom_d(s1, s1, %d bits) is %f ", len * 8, b);
	test_double_result(0, b);
	b = bloom_d(d1, d2, len);
	printf("bloom_d(s1, s2, %d bits) is %f ", len * 8, b);
	test_double_result(0.2, b);
	b = bloom_d(d1, d3, len);
	printf("bloom_d(s1, s3, %d bits) is %f ", len * 8, b);
	test_double_result(0.8, b);
	b = bloom_d(d2, d3, len);
	printf("bloom_d(s2, s3, %d bits) is %f ", len * 8, b);
	test_double_result(0.75, b);
	free(d1);
	free(d2);
	free(d3);

	//test 128 bit digests, create bloom filters
	len = 16;
	d1 = (char *) malloc(len);
	d2 = (char *) malloc(len);
	d3 = (char *) malloc(len);
	if (!d1 || !d2 || !d3)
		fprintf(stderr, "could not allocate digests");

	bloom_create(s1, strlen(s1), d1, len);
	bloom_create(s2, strlen(s2), d2, len);
	bloom_create(s3, strlen(s3), d3, len);

	b = bloom_d(d1, d1, len);
	printf("bloom_d(s1, s1, %d bits) is %f ", len * 8, b);
	test_double_result(0, b);
	b = bloom_d(d1, d2, len);
	printf("bloom_d(s1, s2, %d bits) is %f ", len * 8, b);
	test_double_result(0.5, b);
	b = bloom_d(d1, d3, len);
	printf("bloom_d(s1, s3, %d bits) is %f ", len * 8, b);
	test_double_result(0.83333333333, b);
	b = bloom_d(d2, d3, len);
	printf("bloom_d(s2, s3, %d bits) is %f ", len * 8, b);
	test_double_result(0.91666666666, b);

	return;
}

static void
test_mld(void)
{
	int 	i, x, y, l;
	double	mld;
	char   *needle = "s";
	char   *haystack = " abcdefghijklmnopqrstuvwxyz";
	char   *s1 = "get this party started";
	char   *s2 = "this party is started";
	char   *s3 = "this parte is started";
	char   *s4 = "this partee is started";
	struct 	matrix *m;

	printf("testing needleman_wunsch_distance()\n");

	/* build a matrix of all of the same values for testing */
	m = malloc(sizeof(struct matrix));

	snprintf(m->input, 255, "%s", haystack);
	for (x = 0; x < 255; x++) {
		for (y = 0; y < 255; y++) {
			m->conversion[x][y] = 0.1;
			m->insertion[x][y] = 1.0;
		}
	}

	mld = needleman_wunsch_d(s1, strlen(s1), s1, strlen(s1), m);
	printf("needleman_wunsch_d() returns %f ", mld);
	test_double_result(0.0, mld);

	mld = needleman_wunsch_d(s1, strlen(s1), s2, strlen(s2), m);
	printf("needleman_wunsch_d() returns %f ", mld);
	test_double_result(2.30, mld);

	mld = needleman_wunsch_d(s2, strlen(s2), s3, strlen(s3), m);
	printf("needleman_wunsch_d() returns %f ", mld);
	test_double_result(0.10, mld);

	mld = needleman_wunsch_d(s2, strlen(s2), s4, strlen(s4), m);
	printf("needleman_wunsch_d() returns %f ", mld);
	test_double_result(1.10, mld);

	free(m);

	return;
}


static void
test_jd()
{
	float 	jd;
	char   *j1 = "i like to party";
	char   *j2 = "this party is started";
	char   *j3 = "this parte is started";
	char   *j4 = "this party wasstarted";
	char   *j5 = "z";
	char   *j6 = "";

	printf("testing jaccard_d()\n");

	jd = jaccard_d(j1, strlen(j1), j1, strlen(j1));
	printf("jaccard_d() returns %f ", jd);
	test_double_result(0.0, jd);

	jd = jaccard_d(j1, strlen(j1), j2, strlen(j2));
	printf("jaccard_d() returns %f ", jd);
	test_double_result(-1.0, jd);

	jd = jaccard_d(j2, strlen(j2), j3, strlen(j3));
	printf("jaccard_d() returns %f ", jd);
	test_double_result(0.952381, jd);

	jd = jaccard_d(j2, strlen(j2), j4, strlen(j4));
	printf("jaccard_d() returns %f ", jd);
	test_double_result(0.857143, jd);

	jd = jaccard_d(j2, strlen(j2), j5, strlen(j5));
	printf("jaccard_d() returns %f ", jd);
	test_double_result(-1.0, jd);

	jd = jaccard_d(j5, strlen(j5), j6, strlen(j6));
	printf("jaccard_d() returns %f ", jd);
	test_double_result(-1.0, jd);

	return;
}

void
test_md(void)
{
	float 	md;
	char   *m1 = "this parte is started";
	char   *m2 = "this party is started";
	char   *m3 = "i like to party";
	char   *m4 = "p";
	char   *m5 = "this party";
	char   *m6 = "this party  is started";

	printf("testing minkowski_d()\n");

	md = minkowski_d(m1, strlen(m1), m1, strlen(m1), 1);
	printf("minkowski_d() returns %f ", md);
	test_double_result(0.0, md);

	md = minkowski_d(m1, strlen(m1), m1, strlen(m1), 2);
	printf("minkowski_d() returns %f ", md);
	test_double_result(0.0, md);

	md = minkowski_d(m2, strlen(m2), m1, strlen(m1), 1);
	printf("minkowski_d() returns %f ", md);
	test_double_result(20.0, md);

	md = minkowski_d(m1, strlen(m1), m2, strlen(m2), 1);
	printf("minkowski_d() returns %f ", md);
	test_double_result(20.0, md);

	md = minkowski_d(m1, strlen(m1), m2, strlen(m2), 2);
	printf("minkowski_d() returns %f ", md);
	test_double_result(400.0, md);

	md = minkowski_d(m3, strlen(m3), m4, strlen(m4), 1);
	printf("minkowski_d() returns %f ", md);
	test_double_result(23.0, md);

	md = minkowski_d(m3, strlen(m3), m4, strlen(m4), 4);
	printf("minkowski_d() returns %f ", md);
	test_double_result(6575.0, md);

	md = minkowski_d(m4, strlen(m4), m5, strlen(m5), 1);
	printf("minkowski_d() returns %f ", md);
	test_double_result(18.0, md);

	md = minkowski_d(m2, strlen(m2), m6, strlen(m6), 1);
	printf("minkowski_d() returns %f ", md);
	test_double_result(0.0, md);

	return;
} 

int
main(int argc, char *argv[])
{
	test_hd();
	test_ld();
	test_bloom();
	test_mld();
	test_jd();
	test_md();
	test_dd();

	printf("-----\nEND: %d of %d tests pass\n", num_pass, num_tests);

	return (0);
}
