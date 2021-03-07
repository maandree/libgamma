/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
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
libgamma_x_randr_crtc_get_gamma_ramps16(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps16 *restrict ramps)
{
	xcb_connection_t *restrict connection = this->partition->site->data;
	xcb_randr_get_crtc_gamma_cookie_t cookie;
	xcb_randr_get_crtc_gamma_reply_t *restrict reply;
	xcb_generic_error_t *error;
	uint16_t *restrict red;
	uint16_t *restrict green;
	uint16_t *restrict blue;
  
#ifdef DEBUG
	/* Gamma ramp sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif

	/* Read current gamma ramps */
	cookie = xcb_randr_get_crtc_gamma(connection, *(xcb_randr_crtc_t *)this->data);
	reply = xcb_randr_get_crtc_gamma_reply(connection, cookie, &error);

	/* Check for errors */
	if (error)
		return libgamma_x_randr_internal_translate_error(error->error_code, LIBGAMMA_GAMMA_RAMP_READ_FAILED, 0);

	/* Get current gamma ramps from response */
	red   = xcb_randr_get_crtc_gamma_red(reply);
	green = xcb_randr_get_crtc_gamma_green(reply);
	blue  = xcb_randr_get_crtc_gamma_blue(reply);
	/* Copy over the gamma ramps to our memory */
	memcpy(ramps->red,   red,   ramps->red_size   * sizeof(*red));
	memcpy(ramps->green, green, ramps->green_size * sizeof(*green));
	memcpy(ramps->blue,  blue,  ramps->blue_size  * sizeof(*blue));

	free(reply);
	return 0;
}
