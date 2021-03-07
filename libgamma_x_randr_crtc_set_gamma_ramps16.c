/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
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
libgamma_x_randr_crtc_set_gamma_ramps16(struct libgamma_crtc_state *restrict this, const struct libgamma_gamma_ramps16 *ramps)
{
	xcb_connection_t *restrict connection = this->partition->site->data;
	xcb_void_cookie_t cookie;
	xcb_generic_error_t *restrict error;
#ifdef DEBUG
	/* Gamma ramp sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif

	/* Apply gamma ramps */
	cookie = xcb_randr_set_crtc_gamma_checked(connection, *(xcb_randr_crtc_t*)this->data,
	                                          (uint16_t)ramps->red_size, ramps->red, ramps->green, ramps->blue);
	/* Check for errors */
	error = xcb_request_check(connection, cookie);
	if (error)
		return libgamma_x_randr_internal_translate_error(error->error_code, LIBGAMMA_GAMMA_RAMP_WRITE_FAILED, 0);
	return 0;
}
