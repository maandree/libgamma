/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_x_vidmode_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict this, const libgamma_gamma_ramps16_t *restrict ramps)
{
#ifdef DEBUG
	/* Gamma ramp sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
	/* Apply gamma ramps */
	if (!XF86VidModeSetGammaRamp((Display *)this->partition->site->data, (int)this->partition->partition,
	                             (int)ramps->red_size, ramps->red, ramps->green, ramps->blue))
		return LIBGAMMA_GAMMA_RAMP_WRITE_FAILED;
	return 0;
}
