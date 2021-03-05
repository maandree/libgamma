/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
#include "common.h"


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void
libgamma_linux_drm_crtc_destroy(libgamma_crtc_state_t *restrict this)
{
	(void) this;
}
