/* File rand.c.  Implementation of the pseudo random number generator.
 */

#include "rand.h"

/* Note that the 'long int' size of the machine should be 4 bytes, and the
 * 'long long int' size should be 8 bytes.  Checks should be made by any
 * program using the functions in this file otherwise an overflow could
 * occur when using the values of M and A defined in "rand.h".
 */

/* The seed RAND_X0 is defined in rand.h. */
static long int x = RAND_X0;

/* Get the next pseudo-random number.
 */
long int rand_generate()
{
    /* Overflow of in the calculation RAND_A * x is avoided using a
     * 'long long int' type cast.
     */
    x = ((long long int) RAND_A * x) % RAND_M;

    return x;
}
