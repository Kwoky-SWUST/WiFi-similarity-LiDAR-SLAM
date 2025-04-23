/* $Id: hamming.c,v 1.3 2004/11/29 22:08:48 jose Exp $ */

#include <stdlib.h>
#include <string.h>

/*
   R. W. Hamming, "Error Detecting and Error Correcting Codes", Bell System
   Tech Journal, 9, 147-160, April 1950.

   The Hamming distance H is defined only for strings of the same length.
   For two strings s and t, H(s, t) is the number of places in which the
   two string differ, i.e., have different characters.
 */

int
hamming_d(const void *d1, size_t len1, const void *d2, size_t len2)
{
	int             H = 0, n, m;
	char           *s, *t;

	s = (char *) d1;
	t = (char *) d2;
	n = len1;
	m = len2;

	/* strings must be of equal size and non-zero length */
	if (n == 0 || (n != m))
		return -1;

	/* strings equal? */
	if (strncmp(s, t, n) == 0)
		return 0;

	while (*s != NULL) {
		if (strncmp(s, t, 1) != 0)
			H++;
		s++;
		t++;
	}

	return (H);
}
