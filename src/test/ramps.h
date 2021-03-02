/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_TEST_RAMPS_H
#define LIBGAMMA_TEST_RAMPS_H


#include <stdint.h>


#ifndef __GCC__
# define __attribute__(x)
#endif


/**
 * X macros of all integer gamma ramps
 */
#define LIST_INTEGER_RAMPS  X(ramps8) X(ramps32) X(ramps64) X(ramps16)

/**
 * X macros of all floating-point gamma ramps
 */
#define LIST_FLOAT_RAMPS  X(rampsf) X(rampsd)

/**
 * X macros of all gamma ramps
 */
#define LIST_RAMPS  LIST_FLOAT_RAMPS LIST_INTEGER_RAMPS


/* ramps16 is last because we want to make sure that the gamma ramps are
   preserved exactly on exit, and we assume RandR X is used. */



/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁸ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2⁸ − 1] integer output value
 */
uint8_t invert_ramps8(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2¹⁶ − 1] integer output value
 */
uint16_t invert_ramps16(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2³² − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2³² − 1] integer output value
 */
uint32_t invert_ramps32(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁶⁴ − 1] integer output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 2⁶⁴ − 1] integer output value
 */
uint64_t invert_ramps64(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 1] float output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 1] float output value
 */
float invert_rampsf(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] double precision float encoding
 * value to [0, 1] double precision float output value
 * 
 * @param   encoding  [0, 1] float encoding value
 * @return            [0, 1] float output value
 */
double invert_rampsd(double encoding) __attribute__((const));


#endif
