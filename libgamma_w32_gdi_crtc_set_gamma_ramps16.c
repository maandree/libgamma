/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
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
libgamma_w32_gdi_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict this, const libgamma_gamma_ramps16_t *restrict ramps)
{
#ifdef DEBUG
	/* Windows GDI have fixed gamma ramp sizes */
	if (ramps->  red_size != W32_GDI_GAMMA_RAMP_SIZE ||
	    ramps->green_size != W32_GDI_GAMMA_RAMP_SIZE ||
	    ramps-> blue_size != W32_GDI_GAMMA_RAMP_SIZE)
		return LIBGAMMA_WRONG_GAMMA_RAMP_SIZE;
#endif
	/* Apply gamma ramps */
	if (!SetDeviceGammaRamp(this->data, ramps->red))
		return LIBGAMMA_GAMMA_RAMP_WRITE_FAILED;
	return 0;
}
