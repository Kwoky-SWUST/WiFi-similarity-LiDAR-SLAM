/*	$Id: bloom.c,v 1.6 2004/11/29 21:43:15 jose Exp $ */
/*
    DESCRIPTION:
    A Bloom Filter (BF) is a method of encoding a variable length piece of
    input data to a fixed length signature.  The basic idea is to map
    attributes of the data to specific positions in a binary vector.  First,
    the result vector is initialized to all 0's.  Next, a series of hash
    functions are applied to the data to map some part of the input data
    to a position in the result vector.  Thus, if a is the input data,
    and m is length of the result vector, then hash(a) produces a value in the
    range 0...m-1.

    A similar method encoding data into a fixed length result vector can be
    accomplished using a crytographic digest.  A digest function like MD5
    is designed such that the difference between any two digests does not in
    any way correlate to similar differences in input messages.  This is
    an important property in certain applications however it may also be
    desirable that the difference between digests corresponds to
    differences in the input messages.  Because a bloom filter is derived from
    attributes of the input message, the difference between any two Bloom
    Filter digests corresponds to differences in the input data.

    IMPLEMENTATION:
    This bloom filter implementation is meant take any byte sequence as input
    so a generic hash function is used.  The input stream is divided into
    chunks and hashed using a 32 bit adler modulo m.  In order to get the most
    complete coverage, the chunk N+1 includes all but one of the bytes in
    chunk N and one byte not included in chunk N.  (i.e. the chunk window
    slides to right one byte per hash)

    The difference between any two bloom filter digests is computed by taking
    the logical and of the two digests and computing the number of bit of
    similarity.  The similar count is then divided by maximum of the total
    number of bit in either digest.  This acts to normalize the bit count for
    large and small signatures.  The similarity is then turned into a distance
    by take 1 - normalized bit count.
*/

#ifdef __MINGW32__
#include "sys/types.h"
#else
#include <sys/types.h>
#endif 	/* __MINGW32__ */

#include <stdlib.h>
#if defined ( __APPLE__ ) || ( __FreeBSD__ )
#include <stdint.h>
#endif  /* __APPLE__ || __FreeBSD__ */
#include <string.h>

#include <stdint.h>

#define ADLER_WINDOW		4		// in bytes
#define	ADLER_INC		1		// in bytes

// (C) Donald  W.Gillies, 1992. All rights reserved.You may reuse
// this bitcount() function anywhere you please as long as you retain
// this Copyright Notice.
#define bitready()      register unsigned long tmp
#define bitcount(n)                                                     \
      (tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111), \
      tmp = ((tmp + (tmp >> 3)) & 030707070707),                        \
      tmp =  (tmp + (tmp >> 6)),                                        \
      tmp = (tmp + (tmp >> 12) + (tmp >> 24)) & 077)

/*
 * NOTICE FOR ADLER32 Function (from zlib.h)
 * 
 * Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must
 * not claim that you wrote the original software. If you use this
 * software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required. 2. Altered
 * source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software. 3. This notice may
 * not be removed or altered from any source distribution.
 * 
 * Jean-loup Gailly     jloup@gzip.org
 * Mark Adler		madler@alumni.caltech.edu
 * 
 */
#define BASE 65521L		/* largest prime smaller than 65536 */
#define NMAX 5552
/*
 * NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <=
 * 2^32-1
 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

static uint32_t 
adler32(uint32_t adler, const char *buf, uint32_t len)
{
	uint32_t        s1 = adler & 0xffff;
	uint32_t        s2 = (adler >> 16) & 0xffff;
	int             k;

	if (buf == NULL)
		return 1L;

	while (len > 0) {
		k = len < NMAX ? len : NMAX;
		len -= k;
		while (k >= 16) {
			DO16(buf);
			buf += 16;
			k -= 16;
		}
		if (k != 0)
			do {
				s1 += *buf++;
				s2 += s1;
			} while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}

/*
    Computes the bloom filter digest of data (len bytes long).  digest is a
    preallocated buffer of digest_len bytes long which will hold the
    resulting digest upon succesfully completion. digest_len must be be
    32 bit aligned (i.e. divisible by 4)
*/
void
bloom_create(const void *data, size_t len, const void *digest, size_t digest_len)
{
	int             bit_id, offset;
	uint32_t        adler;
	char           *buf, *d1;

	buf = (char *) data;
	d1 = (char *) digest;
	offset = 0;
	memset((void *)digest, 0, digest_len);
	while (1) {
		adler = adler32(0, buf + offset, ADLER_WINDOW);
		bit_id = adler % (digest_len * 8);
		d1[bit_id / 8] |= (0x1 << (bit_id % 8));

		offset += ADLER_INC;
		if (offset + ADLER_WINDOW > digest_len)
			break;
	}
}

/*
    Computes the distance between any two bloom filter digest of the same
    length.  digest_len must be be 32 bit aligned (i.e. divisible by 4)
*/
double
bloom_d(const void *digest1, const void *digest2, size_t digest_len)
{
	uint32_t       *d1, *d2;
	int             j, d, bit_cnt1, bit_cnt2, b;

	bitready();
	d1 = (uint32_t *) digest1;
	d2 = (uint32_t *) digest2;

	//get the bit count for each digest
	bit_cnt1 = 0;
	for (j = 0; j < digest_len / 4; j++)
		bit_cnt1 += bitcount(d1[j]);
	bit_cnt2 = 0;
	for (j = 0; j < digest_len / 4; j++)
		bit_cnt2 += bitcount(d1[j]);

	//get the bit count of the the logical and of the digest
	d = 0;
	for (j = 0; j < digest_len / 4; j++)
		d += bitcount((d1[j] & d2[j]));

	//return 1 minis the ratio of the bit count over the largest number of
	// bits in either digest
	b = (bit_cnt1 > bit_cnt2) ? bit_cnt1 : bit_cnt2;
	if (d != 0)
		return (1.0 - (double) d / ((double) (b)));
	return 0.0;
}
