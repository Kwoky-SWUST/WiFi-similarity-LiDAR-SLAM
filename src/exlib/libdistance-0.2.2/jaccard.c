/* $Id: jaccard.c,v 1.3 2004/11/29 22:09:02 jose Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Jaccard 1912, "The distribution of the flora of the alpine zone", 
  New Phytologist 11:37-50

  Jaccard Similarity uses word sets from the comparison instances to 
  evaluate similarity. The jaccard similarity penalizes a small number 
  of shared entries ( as a portion of all non-zero entries) more than 
  the Dice coeffient. The Jaccard similarity is frequently used as a 
  similarity measure for chemical compounds.

  This can more easily be described as ( |X & Y| ) / ( | X or Y | )
 */

float
jaccard_d(const void *d1, size_t len1, const void *d2, size_t len2)
{
	float           n, m, same, diff;
	float 		J;
	char           *s, *t;

	s = (char *)d1;
	t = (char *)d2;
	n = len1;
	m = len2;

	/* strings must be of equal size and non-zero length */
	if (n == 0 || (n != m))
		return(-1.0);

	/* strings equal? */
	if (strncmp(s, t, n) == 0)
		return(0.0);

	same = 0;
	diff = 0;
	while (*s != NULL) {
		if (strncmp(s, t, 1) != 0)
			same++;
		else
			diff++;
		s++;
		t++;
	}

	J = ((same)/(diff+same));
	return(1 - J);
}
