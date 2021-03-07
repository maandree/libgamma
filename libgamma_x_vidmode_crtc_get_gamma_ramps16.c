/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_x_vidmode_crtc_get_gamma_ramps16(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps16 *restrict ramps)
{
#ifdef DEBUG
	/* Gamma ramp sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
	/* Read current gamma ramps */
	if (!XF86VidModeGetGammaRamp((Display *)this->partition->site->data, (int)this->partition->partition,
	                             (int)ramps->red_size, ramps->red, ramps->green, ramps->blue))
		return LIBGAMMA_GAMMA_RAMP_READ_FAILED;
	return 0;
}
