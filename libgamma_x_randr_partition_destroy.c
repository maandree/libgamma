/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
#include "common.h"


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void
libgamma_x_randr_partition_destroy(libgamma_partition_state_t *restrict this)
{
	libgamma_x_randr_partition_data_t *restrict data = this->data;
	free(data->crtcs);
	free(data->outputs);
	free(data->crtc_to_output);
	free(data);
}
