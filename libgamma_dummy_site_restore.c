/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_dummy_site_restore(struct libgamma_site_state *restrict this)
{
	struct libgamma_dummy_site *data = this->data;
	size_t i, j;

	if (!libgamma_dummy_internal_configurations.capabilities.site_restore) {
		errno = ENOTSUP;
		return LIBGAMMA_ERRNO_SET;
	}

	for (j = 0; j < data->partition_count; j++)
		for (i = 0; i < data->partitions[j].crtc_count; i++)
			if (libgamma_dummy_internal_crtc_restore_forced(data->partitions[j].crtcs + i))
				return -1;

	return 0;
}
