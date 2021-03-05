/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int
libgamma_quartz_cg_partition_restore(libgamma_partition_state_t *restrict this)
{
	return libgamma_quartz_cg_site_restore(this->site);
}
