/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a partition state
 * and free the partition state pointer
 * 
 * @param  this  The partition state
 */
extern inline void libgamma_partition_free(struct libgamma_partition_state *restrict);
