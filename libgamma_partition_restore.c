/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_partition_restore(libgamma_partition_state_t *restrict this)
{
	switch (this->site->method) {
#define X(CONST, CNAME, DEPTH, RAMPS)\
	case CONST:\
		return libgamma_##CNAME##_partition_restore(this);
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
