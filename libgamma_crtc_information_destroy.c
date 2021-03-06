/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources in an information data structure for a CRTC
 * 
 * @param  this  The CRTC information
 */
void
libgamma_crtc_information_destroy(struct libgamma_crtc_information *restrict this)
{
	free(this->edid);
	free(this->connector_name);
}
