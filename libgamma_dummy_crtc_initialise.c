/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
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
libgamma_dummy_crtc_initialise(struct libgamma_crtc_state *restrict this,
                               struct libgamma_partition_state *restrict partition, size_t crtc)
{
	struct libgamma_dummy_partition *partition_data = partition->data;
	struct libgamma_dummy_crtc *data = &partition_data->crtcs[crtc];
	size_t stop_size;

	this->data = NULL;

	if (crtc >= partition_data->crtc_count)
		return LIBGAMMA_NO_SUCH_CRTC;

	this->data = data;
	data->state = this;

	if (data->info.gamma_depth == -1)
		stop_size = sizeof(float);
	else if (data->info.gamma_depth == -2)
		stop_size = sizeof(double);
	else
		stop_size = (size_t)data->info.gamma_depth / 8;

	data->gamma_red = malloc(data->info.red_gamma_size * stop_size);
	if (!data->gamma_red)
		goto fail;
	data->gamma_green = malloc(data->info.green_gamma_size * stop_size);
	if (!data->gamma_green)
		goto fail;
	data->gamma_blue = malloc(data->info.blue_gamma_size * stop_size);
	if (!data->gamma_blue)
		goto fail;

	return libgamma_dummy_internal_crtc_restore_forced(data);

fail:
	free(data->gamma_red);
	data->gamma_red = NULL;
	free(data->gamma_green);
	data->gamma_green = NULL;
	free(data->gamma_blue);
	data->gamma_blue  = NULL;
	return LIBGAMMA_ERRNO_SET;
}
