/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The index of the CRTC within the partition
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_crtc_initialise(struct libgamma_crtc_state *restrict this, struct libgamma_partition_state *restrict partition, size_t crtc)
{
	this->partition = partition;
	this->crtc = crtc;

	switch (partition->site->method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		return libgamma_##CNAME##_crtc_initialise(this, partition, crtc);
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
