/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_QUARTZ_CG
#include "common.h"


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void
libgamma_quartz_cg_partition_destroy(libgamma_partition_state_t *restrict this)
{
	free(this->data);
}
