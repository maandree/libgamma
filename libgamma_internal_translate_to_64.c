/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Concatenation of all ramps
 */
#define ALL red


/**
 * Preform installation in an `for (i = 0; i < n; i++)`
 * loop and do a `break` afterwords
 */
#define __translate(instruction)  for (i = 0; i < n; i++) instruction;  break


/**
 * Convert a [0, 1] `float` to a full range `uint64_t`
 * and mark sure rounding errors does not cause the
 * value be 0 instead of ~0 and vice versa
 * 
 * @param   value  To `float` to convert
 * @return         The value as an `uint64_t`
 */
static uint64_t
float_to_64(float value)
{
	/* TODO Which is faster? */
  
#if defined(HAVE_INT128) && __WORDSIZE == 64
	/* `__int128` is a GNU C extension, which
	   (because it is not ISO C) emits a warning
	   under -pedantic */
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpedantic"

	/* In GCC we can use `__int128`, this is
	   a signed 128-bit integer. It fits all
	   uint64_t values but also native values,
	   which is a nice because it eleminates
	   some overflow condition tests. It is
	   also more readable. */

	/* Convert to integer */
	__int128 product = (__int128)(value * (float)UINT64_MAX);
	/* Negative overflow */
	if (product > UINT64_MAX)
		return UINT64_MAX;
	/* Positive overflow */
	if (product < 0)
		return 0;
	/* Did not overflow */
	return (uint64_t)product;

# pragma GCC diagnostic pop
#else

	/* If we are not using GCC we cannot be
	   sure that we have `__int128` so we have
	   to use `uint64_t` and perform overflow
	   checkes based on the input value */
  
	/* Convert to integer. */
	uint64_t product = (uint64_t)(value * (float)UINT64_MAX);
	/* Negative overflow,
	   if the input is less than 0.5 but
	   the output is greater then we got
	   -1 when we should have gotten 0 */
	if (value < 0.1f && product > 0xF000000000000000ULL)
		return 0;
	/* Positive overflow,
	   if the input is greater than 0.5
	   but the output is less then we got
	   0 when we should have gotten ~0 */
	else if (value > 0.9f && product < 0x1000000000000000ULL)
		return (uint64_t)~0;
	/* Did not overflow */
	return product;

#endif
}


/**
 * Convert a [0, 1] `double` to a full range `uint64_t`
 * and mark sure rounding errors does not cause the
 * value be 0 instead of ~0 and vice versa
 * 
 * @param   value  To `double` to convert
 * @return         The value as an `uint64_t`
 */
static uint64_t
double_to_64(double value)
{
	/* TODO Which is faster? */

#if defined(HAVE_INT128) && __WORDSIZE == 64
	/* `__int128` is a GNU C extension, which
	   (because it is not ISO C) emits a warning
	   under -pedantic */
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpedantic"

	/* In GCC we can use `__int128`, this is
	   a signed 128-bit integer. It fits all
	   uint64_t values but also native values,
	   which is a nice because it eleminates
	   some overflow condition tests. It is
	   also more readable. */

	/* Convert to integer */
	__int128 product = (__int128)(value * (double)UINT64_MAX);
	/* Negative overflow */
	if (product > UINT64_MAX)
		return UINT64_MAX;
	/* Positive overflow */
	if (product < 0)
		return 0;
	/* Did not overflow */
	return (uint64_t)product;

# pragma GCC diagnostic pop
#else

	/* If we are not using GCC we cannot be
	   sure that we have `__int128` so we have
	   to use `uint64_t` and perform overflow
	   checkes based on the input value. */

	/* Convert to integer. */
	uint64_t product = (uint64_t)(value * (double)UINT64_MAX);
	/* Negative overflow,
	   if the input is less than 0.5 but
	   the output is greater then we got
	   -1 when we should have gotten 0 */
	if (value < (double)0.1f && product > 0xF000000000000000ULL)
		product = 0;
	/* Positive overflow,
	   if the input is greater than 0.5
	   but the output is less then we got
	   0 when we should have gotten ~0 */
	else if ((value > (double)0.9f) && (product < 0x1000000000000000ULL))
		product = (uint64_t)~0;
	/* Did not overflow */
	return product;

#endif
}


/**
 * Convert any set of gamma ramps into a 64-bit integer array with all channels
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes)
 * @param  out    Output array
 * @param  in     Input gamma ramps
 */
void
libgamma_internal_translate_to_64(signed depth, size_t n, uint64_t *restrict out, const gamma_ramps_any_t *restrict in)
{
	size_t i;
	switch (depth) {
	/* Translate integer */
	case  8:  __translate(out[i] = (uint64_t)in->bits8. ALL[i] * UINT64_C(0x0101010101010101));
	case 16:  __translate(out[i] = (uint64_t)in->bits16.ALL[i] * UINT64_C(0x0001000100010001));
	case 32:  __translate(out[i] = (uint64_t)in->bits32.ALL[i] * UINT64_C(0x0000000100000001));
	/* Identity translation */
	case 64:  __translate(out[i] = in->bits64.ALL[i]);
	/* Translate floating point */
	case -1:  __translate(out[i] =  float_to_64(in->float_single.ALL[i]));
	case -2:  __translate(out[i] = double_to_64(in->float_double.ALL[i]));
	default:
		/* This is not possible */
		abort();
		break;
	}
}
