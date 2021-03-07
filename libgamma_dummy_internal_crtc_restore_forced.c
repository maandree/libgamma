/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
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
libgamma_dummy_internal_crtc_restore_forced(struct libgamma_dummy_crtc *restrict data)
{
	size_t rn = data->info.  red_gamma_size;
	size_t gn = data->info.green_gamma_size;
	size_t bn = data->info. blue_gamma_size;
	double rm = (double)(rn - 1);
	double gm = (double)(gn - 1);
	double bm = (double)(bn - 1);
	size_t i;

	if (!data->gamma_red)
		return 0;

#define RESET_RAMPS(TYPE, MAX)\
	do {\
		TYPE *red   = data->gamma_red;\
		TYPE *green = data->gamma_green;\
		TYPE *blue  = data->gamma_blue;\
		for (i = 0; i < rn; i++) red  [i] = (TYPE)((double)(MAX) * ((double)i / rm));\
		for (i = 0; i < gn; i++) green[i] = (TYPE)((double)(MAX) * ((double)i / gm));\
		for (i = 0; i < bn; i++) blue [i] = (TYPE)((double)(MAX) * ((double)i / bm));\
	} while (0)

	if      (data->info.gamma_depth ==  8) RESET_RAMPS(uint8_t,  INT8_MAX);
	else if (data->info.gamma_depth == 16) RESET_RAMPS(uint16_t, INT16_MAX);
	else if (data->info.gamma_depth == 32) RESET_RAMPS(uint32_t, INT32_MAX);
	else if (data->info.gamma_depth == 64) RESET_RAMPS(uint64_t, INT64_MAX);
	else if (data->info.gamma_depth == -1) RESET_RAMPS(float,    1);
	else                                   RESET_RAMPS(double,   1);

#undef RESET_RAMPS

	return 0;
}
