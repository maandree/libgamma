/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void
libgamma_w32_gdi_crtc_destroy(libgamma_crtc_state_t *restrict this)
{
	if (this->data)
		ReleaseDC(NULL, this->data);
}
