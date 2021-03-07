/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_QUARTZ_CG
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_quartz_cg_crtc_set_gamma_rampsf(struct libgamma_crtc_state *restrict this, const struct libgamma_gamma_rampsf *restrict ramps)
{
	CGDirectDisplayID *restrict crtcs = this->partition->data;
	CGDirectDisplayID crtc_id = crtcs[this->crtc];
	CGError r;
#ifdef DEBUG
	/* Gamma ramps sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
	/* Apply gamma ramps */
	r = CGSetDisplayTransferByTable(crtc_id, (uint32_t)ramps->red_size, ramps->red, ramps->green, ramps->blue);
	return r == kCGErrorSuccess ? 0 : LIBGAMMA_GAMMA_RAMP_WRITE_FAILED;
}
