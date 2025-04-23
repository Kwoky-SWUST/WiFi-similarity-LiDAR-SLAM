/* 	$Id: levenshtein.c,v 1.2 2004/11/29 22:08:48 jose Exp $ */

#include <stdlib.h>
#include <string.h>

#include "distance.h"

/*
   V. I. Levenshtein, "Binary codes capable of correcting deletions,
   insertions and reversals", Doklady Akademii Nauk SSSR, 4, 163, 845-848,
   1965.

   Levenshtein distance (LD) is a measure of the similarity between two
   strings, which we will refer to as the source string (s) and the target
   string (t). The distance is the number of deletions, insertions, or
   substitutions required to transform s into t.

   by Lorenzo Seidenari (sixmoney@virgilio.it)

   see: http://www.merriampark.com/ld.htm
 */

/* Compute levenshtein distance between d1 and d2 */

int
levenshtein_d(const void *d1, size_t len1, const void *d2, size_t len2)
{
	int             k, i, j, n, m, cost, *d, distance, a, b, c;
	char           *s, *t;

	//Step 1
	s = (char *) d1;
	t = (char *) d2;
	n = len1;
	m = len2;
	if (n != 0 && m != 0) {
		d = malloc((sizeof(int)) * (m + 1) * (n + 1));
		m++;
		n++;
		//Step 2
		for (k = 0; k < n; k++)
			d[k] = k;
		for (k = 0; k < m; k++)
			d[k * n] = k;
			//Step 3 and 4
			for (i = 1; i < n; i++)
				for (j = 1; j < m; j++) {
					//Step 5
					if (s[i - 1] == t[j - 1])
						cost = 0;
					else
						cost = 1;
					//Step 6
					a = d[(j - 1) * n + i] + 1;
					b = d[j * n + i - 1] + 1;
					c = d[(j - 1) * n + i - 1] + cost;
					d[j * n + i] = (min(a,(min(b,c))));
				}
			distance = d[n * m - 1];
		free(d);
		return distance;
	} else
		return (max(m,n));
	// return the full string cost if one is zero length
}
