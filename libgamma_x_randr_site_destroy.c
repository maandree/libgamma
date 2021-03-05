/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_x_randr_site_destroy(libgamma_site_state_t *restrict this)
{
	xcb_disconnect((xcb_connection_t *)this->data);
}
