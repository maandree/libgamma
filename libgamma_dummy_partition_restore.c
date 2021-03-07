/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_dummy_partition_restore(struct libgamma_partition_state *restrict this)
{
	struct libgamma_dummy_partition *data = this->data;
	size_t i;

	if (!libgamma_dummy_internal_configurations.capabilities.partition_restore) {
		errno = ENOTSUP;
		return LIBGAMMA_ERRNO_SET;
	}

	for (i = 0; i < data->crtc_count; i++)
		if (libgamma_dummy_internal_crtc_restore_forced(data->crtcs + i) < 0)
			return -1;

	return 0;
}
