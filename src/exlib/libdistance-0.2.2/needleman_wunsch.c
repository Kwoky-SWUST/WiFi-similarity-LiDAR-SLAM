/*	$Id: needleman_wunsch.c,v 1.1 2004/11/29 21:44:48 jose Exp $ */

#include <stdlib.h>
#include <string.h>

#include "distance.h"

/**
   S. B. Needleman and C. D. Wunsch, "A general method applicable to the
   search for similarities in the amino acid sequence of two proteins", Jrnl
   Molec Biol, 48, 443-453, 1970.

   calculate the cost between two inputs using a cost matrix. this
   matrix carries with it the costs for any conversions and insertions.
   for example, going from A to a may be low cost, but A to B may be
   high cost.
 */

double
needleman_wunsch_d(const void *d1, size_t len1, const void *d2, size_t len2, struct matrix *mt)
{
	double 		cost, a, b, c, *d, distance;
	int		k, i, j, n, m, from, to;
	char           *s, *t;

	//Step 1
	s = (char *) d1;
	t = (char *) d2;
	n = len1;
	m = len2;
	if (n != 0 && m != 0) {
		d = malloc((sizeof(double)) * (m + 1) * (n + 1));
		m++;
		n++;
		//Step 2
		for (k = 0; k < n; k++) {
			from = s[0];
			to = t[k];
			d[k] = mt->insertion[from][to];
		}
		for (k = 0; k < m; k++) {
			from = s[k];
			to = t[0];
			d[k * n] = mt->insertion[from][to];
		}
		d[0] = 0;		// XXX
		//Step 3 and 4
		for (i = 1; i < n; i++)
			for (j = 1; j < m; j++) {
				from = s[i - 1];
				to = t[j - 1];
				//Step 5
				if (s[i - 1] == t[j - 1])
					cost = 0;
				else
					cost = mt->conversion[from][to];
				//Step 6
				a = d[(j - 1) * n + i] + mt->insertion[from][to];
				b = d[j * n + i - 1] + mt->insertion[from][to];
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
