/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
#include "common.h"


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void
libgamma_x_vidmode_crtc_destroy(struct libgamma_crtc_state *restrict this)
{
	(void) this;
}
