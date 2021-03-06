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
#define TRANSLATE(instruction)  for (i = 0; i < n; i++) instruction;  break


/**
 * Undo the actions of `libgamma_internal_translate_to_64`
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes)
 * @param  out    Output gamma ramps
 * @param  in     Input array, may be modified
 */
void
libgamma_internal_translate_from_64(signed depth, size_t n, union gamma_ramps_any *restrict out, const uint64_t *restrict in)
{
	size_t i;
	switch (depth) {
	/* Translate integer */
	case  8:  TRANSLATE(out->bits8. ALL[i] =  (uint8_t)(in[i] / UINT64_C(0x0101010101010101)));
	case 16:  TRANSLATE(out->bits16.ALL[i] = (uint16_t)(in[i] / UINT64_C(0x0001000100010001)));
	case 32:  TRANSLATE(out->bits32.ALL[i] = (uint32_t)(in[i] / UINT64_C(0x0000000100000001)));
	/* Identity translation */
	case 64:  TRANSLATE(out->bits64.ALL[i] = in[i]);
	/* Translate floating point */
	case -1:  TRANSLATE(out->float_single.ALL[i] =  (float)in[i] /  (float)UINT64_MAX);
	case -2:  TRANSLATE(out->float_double.ALL[i] = (double)in[i] / (double)UINT64_MAX);
	default:
		/* This is not possible */
		abort();
		break;
	}
}
