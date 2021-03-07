/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_site_destroy(struct libgamma_site_state *restrict this)
{
	switch (this->method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		libgamma_##CNAME##_site_destroy(this);\
		break;
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		break;
	}
	free(this->site);
}
