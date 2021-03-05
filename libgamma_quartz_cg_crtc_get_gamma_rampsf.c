/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_quartz_cg_crtc_get_gamma_rampsf(libgamma_crtc_state_t *restrict this, libgamma_gamma_rampsf_t *restrict ramps)
{
	CGDirectDisplayID *restrict crtcs = this->partition->data;
	CGDirectDisplayID crtc_id = crtcs[this->crtc];
	uint32_t gamma_size_out;
	CGError r;
#ifdef DEBUG
	/* Gamma ramps sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
	/* Read current gamma ramps */
	r = CGGetDisplayTransferByTable(crtc_id, (uint32_t)ramps->red_size, ramps->red, ramps->green, ramps->blue, &gamma_size_out);
	if (r != kCGErrorSuccess)
		return LIBGAMMA_GAMMA_RAMP_READ_FAILED;
	/* I hope that it will not actually ever change,
	   but it does return the the gamma ramp size despite
	   that it can be queried without querying for more */
	if (gamma_size_out != ramps->red_size)
		return LIBGAMMA_GAMMA_RAMP_SIZE_CHANGED;
	return 0;
}
