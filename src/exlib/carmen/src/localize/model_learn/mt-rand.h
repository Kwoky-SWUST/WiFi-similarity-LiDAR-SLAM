//
// mt-rand.h
//
//
// Header file for mt-rand.c

#ifndef MODEL_LEARN_MT_RAND_H_1392498
#define MODEL_LEARN_MT_RAND_H_1392498

#ifndef INLINE
#if defined(__GNUC__) || defined(__DECC)
#define INLINE inline
#else
#define INLINE
#endif
#endif

typedef unsigned long uint32;

double MTrandDec(void);
uint32 randomMT(void);
uint32 reloadMT(void);
void seedMT(uint32 seed);

#endif
