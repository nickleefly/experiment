/*** File timing.c - Implements functions used for timing algorithms. ***/
/*
 *   Shane Saunders
 */
#include <stdio.h>
#include <stdlib.h>
#include "timing.h"
/* Refer to timing.h for a description of each functions use. */

/* The header file allows the user to specify the clock function that
 * clockval() points to:
 *     (default)        clock()       - ANSI C clock() function.
 *     USE_PTIME        gethrvtime()  - High resolution time in nanoseconds
 *                                      when ptime utility is used.  (Solaris)
 *     USE_UCLOCK       uclock()      - More accurate than clock().
 *                                      (If available)
 */


/*** Global Constants ***/
  /* Also declared in the header file. */

#if USE_PTIME
clockval_t (*getclock)(void) = gethrvtime;  /* Pointer to clock function. */
const clockval_t CLOCK_DIV = 1000000000;  /* Clocks per sec. */
#elif USE_UCLOCK
clockval_t (*getclock)(void) = uclock;  /* Pointer to clock function. */
const clockval_t CLOCK_DIV = UCLOCKS_PER_SEC;  /* Clocks per sec. */
#else
clockval_t (*getclock)(void) = clock;  /* Pointer to clock function. */
const clockval_t CLOCK_DIV = CLOCKS_PER_SEC;  /* Clocks per sec. */
#endif


/*** Variables Shared Between Functions in this File ***/

clockval_t start_time, total_time;


/*** Timer Function Defintions ***/

void timer_reset()
{
    total_time = 0;
}

void timer_start()
{
    start_time = getclock();
}

clockval_t timer_stop()
{
    clockval_t stop_time, elapsed_time;

    stop_time = getclock();
    elapsed_time = stop_time - start_time;
    start_time = stop_time;
    total_time += elapsed_time;
    return elapsed_time;
}

clockval_t timer_total()
{
    return total_time;
}

void timer_print(char *fmt, int div)
{
    double scaled_time;
    
    scaled_time = (((double)total_time / div) / CLOCK_DIV) * 1000;
    printf(fmt, scaled_time);
}


/*** Functions Definitions for Recording Multiple Time Measurements ***/

timing_t *timing_alloc(int n)
{
    timing_t *t;

    t = malloc(sizeof(timing_t));
    t->n = n;
    t->totals = calloc(n, sizeof(clockval_t));
    return t;
}

void timing_free(timing_t *t)
{
    free(t->totals);
    free(t);
}

void timing_reset(timing_t *t)
{
    clockval_t *totals;
    int i;

    totals = t->totals;
    for(i = 0; i < t->n; i++) {
        totals[i] = 0;
    }
}

void timing_reset1(timing_t *t, int i)
{
    t->totals[i] = 0;
}

void timing_start() {
    start_time = getclock();
}

clockval_t timing_stop(timing_t *t, int i) {
    clockval_t stop_time, elapsed_time;

    stop_time = getclock();
    elapsed_time = stop_time - start_time;
    start_time = stop_time;
    t->totals[i] += elapsed_time;
    return elapsed_time;
}

clockval_t timing_total(timing_t *t, int i) {
    return t->totals[i];
}

void timing_print(timing_t *t, char *fmt, int div)
{    
    clockval_t *totals;
    double scaled_time;
    int i;

    totals = t->totals;
    for(i = 0; i < t->n; i++) {
        scaled_time = (((double)totals[i] / div) / CLOCK_DIV) * 1000;
        printf(fmt, scaled_time);
    }
}

void timing_print1(timing_t *t, int i, char *fmt, int div)
{
    double scaled_time;
    
    scaled_time = (((double)t->totals[i] / div) / CLOCK_DIV) * 1000;
    printf(fmt, scaled_time);
}

clockval_t timing_sub(timing_t *t)
{
    clockval_t *totals;
    clockval_t stop_time, elapsed_time;
    int i;

    stop_time = getclock();
    elapsed_time = stop_time - start_time;
    start_time = stop_time;
    
    totals = t->totals;
    for(i = 0; i < t->n; i++) {	
        totals[i] += elapsed_time;
    }
    
    return elapsed_time;
}
