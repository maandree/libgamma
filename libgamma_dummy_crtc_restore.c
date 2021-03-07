/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_dummy_crtc_restore(struct libgamma_crtc_state *restrict this)
{
	if (!libgamma_dummy_internal_configurations.capabilities.crtc_restore) {
		errno = ENOTSUP;
		return LIBGAMMA_ERRNO_SET;
	}
	return libgamma_dummy_internal_crtc_restore_forced(this->data);
}
