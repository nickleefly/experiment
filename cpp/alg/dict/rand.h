/* This is the header file, "rand.h", which supplies a multiplicative linear
 * congruential pseudo-random number generator which uses
 * M = 2 147 483 647 (2^31 - 1) and A = 950 706 376.
 *
 * This random number generator is known to produce a full cycle of random
 * numbers, and the sequence of numbers produced has both good geometrical and
 * good statistical properties.
 *
 * Refer to Fishman and Moore, "An exhaustive analysis of multiplicative
 * congruential random number generators with modulus 2^31 - 1", SIAM Journal
 * on Scientific and Statistical Computing, 1986, volume 7.
 *
 * Overflows are avoided using the (long long int) type, see below.
 */
#ifndef RAND_H
#define RAND_H

#define RAND_A 950706376
#define RAND_M 2147483647
#define RAND_X0 111111111

/* Note that the 'long int' size of the machine should be 4 bytes, and the
 * 'long long int' size should be 8 bytes.  Checks should be made by any
 * program using the functions in this file otherwise an overflow could
 * occur when using the values of M and A defined above.
 */
#define RAND_LONG 4
#define RAND_LONGLONG 8


/* Get the next pseudo-random number.
 */
long int rand_generate();


#endif
