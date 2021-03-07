/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_crtc_restore(struct libgamma_crtc_state *restrict this)
{
	switch (this->partition->site->method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		return libgamma_##CNAME##_crtc_restore(this);
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
