/* $Id: minkowski.c,v 1.4 2004/11/30 00:19:36 jose Exp $ */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "distance.h"

/*
   The Minkowski distance is related to the geometric distance between
   two strings point by point. The argument "power" can be used to
   generate the Manhattan distance (when power = 1) or Euclidian
   distance (when power = 2). 

   A disadvantage of the Minkowski method is that if one element in the 
   vectors has a wider range than the other elements then that large 
   range may 'dilute' the distances of the small-range elements.
 */

float
minkowski_d(const void *d1, size_t len1, const void *d2, size_t len2, 
	    int power)
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
#define COST_UNQUAL pow(fabs(s[i - 1] - t[j - 1]), power)
					if (s[i - 1] == t[j - 1])
						cost = 0;
					else
						cost = COST_UNQUAL;
					//Step 6
					a = d[(j - 1) * n + i] + COST_UNQUAL;
					b = d[j * n + i - 1] + COST_UNQUAL;
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
