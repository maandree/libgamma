/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void
libgamma_dummy_crtc_destroy(libgamma_crtc_state_t *restrict this)
{
	libgamma_dummy_crtc_t *data = this->data;
	if (data) {
		free(data->gamma_red);
		data->gamma_red = NULL;
		free(data->gamma_green);
		data->gamma_green = NULL;
		free(data->gamma_blue);
		data->gamma_blue = NULL;
	}
}
