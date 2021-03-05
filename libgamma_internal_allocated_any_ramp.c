/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Just an arbitrary version
 */
#define ANY bits64


/**
 * Allocate and initalise a gamma ramp with any depth
 * 
 * @param   ramps_sys  Output gamma ramps
 * @param   ramps      The gamma ramps whose sizes should be duplicated
 * @param   depth      The depth of the gamma ramps to allocate,
 *                     `-1` for `float`, `-2` for `double`
 * @param   elements   Output reference for the grand size of the gamma ramps
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_internal_allocated_any_ramp(gamma_ramps_any_t *restrict ramps_sys, const gamma_ramps_any_t *restrict ramps,
                                     signed depth, size_t *restrict elements)
{
	/* Calculate the size of the allocation to do */
	size_t d, n = ramps->ANY.red_size + ramps->ANY.green_size + ramps->ANY.blue_size;
	switch (depth) {
	case  8:  d = sizeof(uint8_t);   break;
	case 16:  d = sizeof(uint16_t);  break;
	case 32:  d = sizeof(uint32_t);  break;
	case 64:  d = sizeof(uint64_t);  break;
	case -1:  d = sizeof(float);     break;
	case -2:  d = sizeof(double);    break;
	default:
		return errno = EINVAL, LIBGAMMA_ERRNO_SET;
	}

	/* Copy the gamma ramp sizes */
	ramps_sys->ANY = ramps->ANY;
	/* Allocate the new ramps */
#ifdef HAVE_LIBGAMMA_METHOD_LINUX_DRM
	/* Valgrind complains about us reading uninitialize memory if we just use malloc */
	ramps_sys->ANY.red = calloc(n, d);
#else
	ramps_sys->ANY.red = malloc(n * d);
#endif
	ramps_sys->ANY.green = (void *)&((char *)ramps_sys->ANY.  red)[ramps->ANY.  red_size * d / sizeof(char)];
	ramps_sys->ANY.blue  = (void *)&((char *)ramps_sys->ANY.green)[ramps->ANY.green_size * d / sizeof(char)];

	/* Report the total gamma ramp size */
	*elements = n;
	/* Report successfulness */
	return ramps_sys->ANY.red ? 0 : LIBGAMMA_ERRNO_SET;
}
