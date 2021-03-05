/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_quartz_cg_site_destroy(libgamma_site_state_t *restrict this)
{
	(void) this;
	close_fake_quartz_cg();
}
