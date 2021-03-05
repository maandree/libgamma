/* See LICENSE file for copyright and license details. */
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
libgamma_linux_drm_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict this, const libgamma_gamma_ramps16_t *restrict ramps)
{
	libgamma_drm_card_data_t *restrict card = this->partition->data;
	int r;
#ifdef DEBUG
	/* Gamma ramp sizes are identical but not fixed */
	if (ramps->red_size != ramps->green_size || ramps->red_size != ramps->blue_size)
		return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif

	/* Apply gamma ramps */
	r = drmModeCrtcSetGamma(card->fd, (uint32_t)(size_t)this->data,
	                        (uint32_t)ramps->red_size, ramps->red, ramps->green, ramps->blue);
	/* Check for errors */
	if (r) {
		switch (errno) {
		case EACCES:
		case EAGAIN:
		case EIO:
			/* Permission denied errors must be ignored, because we do not
			 * have permission to do this while a display server is active.
			 * We are also checking for some other error codes just in case. */
		case EBUSY:
		case EINPROGRESS:
			/* It is hard to find documentation for DRM (in fact all of this is
			 * just based on the functions names and some testing,) perhaps we
			 * could get this if we are updating to fast. */
			break;
		case EBADF:
		case ENODEV:
		case ENXIO:
			/* TODO: I have not actually tested removing my graphics card or,
			 *       monitor but I imagine either of these is what would happen. */
			return LIBGAMMA_GRAPHICS_CARD_REMOVED;

		default:
			return LIBGAMMA_ERRNO_SET;
		}
	}
	return 0;
}
