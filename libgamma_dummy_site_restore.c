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
libgamma_dummy_site_restore(libgamma_site_state_t *restrict this)
{
	libgamma_dummy_site_t *data = this->data;
	size_t i, j;

	if (!libgamma_dummy_internal_configurations.capabilities.site_restore) {
		errno = ENOTSUP;
		return LIBGAMMA_ERRNO_SET;
	}

	for (j = 0; j < data->partition_count; j++)
		for (i = 0; i < data->partitions[j].crtc_count; i++)
			if (libgamma_dummy_internal_crtc_restore_forced(data->partitions[j].crtcs + i) < 0)
				return -1;

	return 0;
}
