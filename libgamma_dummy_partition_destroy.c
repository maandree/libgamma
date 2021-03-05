/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void
libgamma_dummy_partition_destroy(libgamma_partition_state_t *restrict this)
{
	libgamma_dummy_partition_t *data = this->data;
	size_t i;

	if (data) {
		for (i = 0; i < data->crtc_count; i++) {
			free(data->crtcs[i].info.edid);
			free(data->crtcs[i].info.connector_name);
		}
		free(data->crtcs);
		data->crtcs = NULL;
	}
}
