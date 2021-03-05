/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * and ignore the method's capabilities
 * 
 * @param   this  The CRTC data
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_dummy_internal_crtc_restore_forced(libgamma_dummy_crtc_t *restrict data)
{
	size_t rn = data->info.  red_gamma_size;
	size_t gn = data->info.green_gamma_size;
	size_t bn = data->info. blue_gamma_size;
	size_t i;

	if (!data->gamma_red)
		return 0;

#define __reset_ramps(TYPE, MAX)\
	do {\
		TYPE *red   = data->gamma_red;\
		TYPE *green = data->gamma_green;\
		TYPE *blue  = data->gamma_blue;\
		for (i = 0; i < rn; i++) red  [i] = (TYPE)((double)(MAX) * ((double)i / (double)(rn - 1)));\
		for (i = 0; i < gn; i++) green[i] = (TYPE)((double)(MAX) * ((double)i / (double)(gn - 1)));\
		for (i = 0; i < bn; i++) blue [i] = (TYPE)((double)(MAX) * ((double)i / (double)(bn - 1)));\
	} while (0)

	if      (data->info.gamma_depth ==  8) __reset_ramps(uint8_t,  INT8_MAX);
	else if (data->info.gamma_depth == 16) __reset_ramps(uint16_t, INT16_MAX);
	else if (data->info.gamma_depth == 32) __reset_ramps(uint32_t, INT32_MAX);
	else if (data->info.gamma_depth == 64) __reset_ramps(uint64_t, INT64_MAX);
	else if (data->info.gamma_depth == -1) __reset_ramps(float,    1);
	else                                   __reset_ramps(double,   1);

#undef __reset_ramps

	return 0;
}
