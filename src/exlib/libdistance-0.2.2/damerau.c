/* $Id: damerau.c,v 1.1 2004/11/29 21:43:35 jose Exp $ */
/* 

   Fred J. Damerau, "A technique for computer detection and correction of
   spelling errors", Communications of the ACM, 3, 7, 171-176, March 1964.

   The Damerau Edit Distance is identical to the Levenshtein edit distance, 
   except that it also allows the operation of transposing (swapping) two 
   adjacent characters at no cost.

   based on code from Lorenzo Seidenari (sixmoney@virgilio.it)
   see: http://www.merriampark.com/ld.htm
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "distance.h"

/* Compute damerau distance between d1 and d2 */

int
damerau_d(const void *d1, size_t len1, const void *d2, size_t len2)
{
	int             k, i, j, n, m, cost, *d, distance, a, b, c;
	char           *s, *t;	

	//Step 1
	s = (char *) d1;
	t = (char *) d2;
	n = len1;
	m = len2;
	if (n != 0 && m != 0) {
		static int swap;

		d = malloc((sizeof(int)) * (m + 1) * (n + 1));
		m++;
		n++;
		//Step 2
		for (k = 0; k < n; k++)
			d[k] = k;
		for (k = 0; k < m; k++)
			d[k * n] = k;
		//Step 3 and 4
		for (i = 1; i < n; i++) {
			for (j = 1; j < m; j++) {
				//Step 5
				//modified from LD to tolerate
				//adjascent character swaps.
				if (s[i - 1] == t[j - 1])
					cost = 0;
				else if ((s[i - 1] == t[j]) &&
					 (s[i] == t[j - 1])) {
					/* tolerate swapped adjascent chars */
					swap = 1;
					cost = 0;
				} else if ((swap) && (s[i - 2] == t[j - 1]) &&
					   (s[i - 1] == t[j - 2])) {
					/* next pass screwed up, reset swap */
					cost = 0;
					swap = 0;	/* turn off */
				} else
					cost = 1;
				//Step 6
				a = d[(j - 1) * n + i] + 1;
				b = d[j * n + i - 1] + 1;
				c = d[(j - 1) * n + i - 1] + cost;
				d[j * n + i] = (min(a,(min(b,c))));
			}
		}
		distance = d[n * m - 1];
		free(d);
		return distance;
	} else
		return (max(m,n));
	// return the full string cost if one is zero length
}
