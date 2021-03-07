/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
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
libgamma_linux_drm_crtc_initialise(struct libgamma_crtc_state *restrict this,
                                   struct libgamma_partition_state *restrict partition, size_t crtc)
{
	struct libgamma_drm_card_data *restrict card = partition->data;
	if (crtc >= partition->crtcs_available)
		return LIBGAMMA_NO_SUCH_CRTC;
	this->data = (void *)(size_t)card->res->crtcs[crtc];
	return 0;
}
