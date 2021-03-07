/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void
libgamma_crtc_destroy(struct libgamma_crtc_state *restrict this)
{
	switch (this->partition->site->method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		libgamma_##CNAME##_crtc_destroy(this);\
		break;
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		break;
	}
}
