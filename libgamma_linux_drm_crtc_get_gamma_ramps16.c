/* See LICENSE file for copyright and license details. */
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
libgamma_linux_drm_crtc_get_gamma_ramps16(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps16_t *restrict ramps)
{
	libgamma_drm_card_data_t *restrict card = this->partition->data;
	int r;
#ifdef DEBUG
	/* Gamma ramp sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
	/* Read current gamma ramps */
	r = drmModeCrtcGetGamma(card->fd, (uint32_t)(size_t)this->data, (uint32_t)ramps->red_size,
				ramps->red, ramps->green, ramps->blue);
	return r ? LIBGAMMA_GAMMA_RAMP_READ_FAILED : 0;
}
