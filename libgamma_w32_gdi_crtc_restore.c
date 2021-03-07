/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
#include "common.h"


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_w32_gdi_crtc_restore(struct libgamma_crtc_state *restrict this)
{
	(void) this;
	errno = ENOTSUP;
	return LIBGAMMA_ERRNO_SET;
}
