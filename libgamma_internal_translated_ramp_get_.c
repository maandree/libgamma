/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Just an arbitrary version
 */
#define ANY bits64


/**
 * Get the current gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to fill with the current values
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used read the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
int
libgamma_internal_translated_ramp_get_(libgamma_crtc_state_t *restrict this, gamma_ramps_any_t *restrict ramps,
                                       signed depth_user, signed depth_system, get_ramps_any_fun *fun)
{
	size_t n;
	int r;
	gamma_ramps_any_t ramps_sys;
	uint64_t *restrict ramps_full;
  
	/* Allocate ramps with proper data type */
	if ((r = libgamma_internal_allocated_any_ramp(&ramps_sys, ramps, depth_system, &n)))
		return r;

	/* Fill the ramps */
	if ((r = fun(this, &ramps_sys))) {
		free(ramps_sys.ANY.red);
		return r;
	}

	/* Allocate intermediary ramps */
	ramps_full = malloc(n * sizeof(uint64_t));
	if (!ramps_full) {
		free(ramps_sys.ANY.red);
		return LIBGAMMA_ERRNO_SET;
	}

	/* Translate ramps to 64-bit integers */
	libgamma_internal_translate_to_64(depth_system, n, ramps_full, &ramps_sys);
	free(ramps_sys.ANY.red);

	/* Translate ramps to the user's format */
	libgamma_internal_translate_from_64(depth_user, n, ramps, ramps_full);
	free(ramps_full);
	return 0;
}
