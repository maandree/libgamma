/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
#include "common.h"


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void
libgamma_linux_drm_partition_destroy(struct libgamma_partition_state *restrict this)
{
	struct libgamma_drm_card_data *restrict data = this->data;
	libgamma_linux_drm_internal_release_connectors_and_encoders(data);
	if (data->res)
		drmModeFreeResources(data->res);
	if (data->fd >= 0)
		close(data->fd);
	free(data);
}
