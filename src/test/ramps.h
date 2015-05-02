/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014, 2015  Mattias Andrée (maandree@member.fsf.org)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
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
 * Test mapping function from [0, 1] float encoding value to [0, 2⁸ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2⁸ − 1] integer output value.
 */
uint8_t invert_ramps8(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2¹⁶ − 1] integer output value.
 */
uint16_t invert_ramps16(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2³² − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2³² − 1] integer output value.
 */
uint32_t invert_ramps32(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁶⁴ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2⁶⁴ − 1] integer output value.
 */
uint64_t invert_ramps64(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] float encoding value to [0, 1] float output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 1] float output value.
 */
float invert_rampsf(float encoding) __attribute__((const));

/**
 * Test mapping function from [0, 1] double precision float encoding
 * value to [0, 1] double precision float output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 1] float output value.
 */
double invert_rampsd(double encoding) __attribute__((const));


#endif

