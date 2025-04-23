/*	$Id: distance.h,v 1.7 2004/10/10 09:12:18 jose Exp $ */
#include <sys/cdefs.h>

__BEGIN_DECLS

/* cost matrix for needleman-wunsch distance */
struct matrix {
	char	input[255];		/* input character */
	float	conversion[255][255];	/* conversion cost, in, out positions */
	float	insertion[255][255];	/* cost of inserting in any position vs what you're inserting before */
};

#ifndef max
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif	/* max */

#ifndef min
#define min(a,b) ((a) <= (b) ? (a) : (b))
#endif	/* min */

/* claculate the edit distance, unit costs for all insertions/conversions */
int     levenshtein_d(const void *d1, size_t len1, const void *d2,
    size_t len2);
/* calculate the damerau distance, like LD but tolerate adjascent swaps */
int 	damerau_d(const void *d1, size_t len1, const void *d2, size_t len2);
/* calculate the hamming distance */
int     hamming_d(const void *d1, size_t len1, const void *d2,
    size_t len2);
/* create a bloom filter for any piece of data */
void    bloom_create(const void *data, size_t len, const void *digest,
    size_t digest_len);
/* calculae the bloom filter distance between two digests */
double  bloom_d(const void *digest1, const void *digest2,
    size_t digest_len);
/* calculate a variable cost edit distance */
double  needleman_wunsch_d(const void *d1, size_t len1, const void *d2, 
    size_t len2, struct matrix *m);
/* calculate the jaccard distance between two strings */
float	jaccard_d(const void *d1, size_t len1, const void *d2,
    size_t len2);
/* calculate the minkowski distance between two strings */
float 	minkowski_d(const void *d1, size_t len1, const void *d2, 
    size_t len2, int power);


/* useful shortcuts */
#define MANHATTAN_D(d1, len1, d2, len2)				\
	minkowski_d(d1, len1, d2, len2, 1)
#define EUCLID_D(d1, len1, d2, len2)				\
	minkowski_d(d1, len1, d2, len2, 2)

__END_DECLS
