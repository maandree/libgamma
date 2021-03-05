/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
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
