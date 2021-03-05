/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_QUARTZ_CG
#include "common.h"



/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The the index of the CRTC within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_quartz_cg_crtc_initialise(libgamma_crtc_state_t *restrict this,
                                   libgamma_partition_state_t *restrict partition, size_t crtc)
{
	(void) this;
	return crtc < partition->crtcs_available ? 0 : LIBGAMMA_NO_SUCH_CRTC;
}
