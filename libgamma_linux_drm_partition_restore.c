/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
#include "common.h"


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings.
 * 
 * @param   this  The partition state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int
libgamma_linux_drm_partition_restore(struct libgamma_partition_state *restrict this)
{
	(void) this;
	errno = ENOTSUP;
	return LIBGAMMA_ERRNO_SET;
}
