/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void
libgamma_partition_destroy(libgamma_partition_state_t *restrict this)
{
	switch (this->site->method) {
#define X(CONST, CNAME, DEPTH, RAMPS)\
	case CONST:\
		libgamma_##CNAME##_partition_destroy(this);\
		break;
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		break;
	}
}
