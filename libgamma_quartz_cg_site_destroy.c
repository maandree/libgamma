/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_QUARTZ_CG
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_quartz_cg_site_destroy(struct libgamma_site_state *restrict this)
{
	(void) this;
	close_fake_quartz_cg();
}
