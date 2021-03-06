/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_x_vidmode_site_destroy(struct libgamma_site_state *restrict this)
{
	XCloseDisplay((Display *)this->data);
}
