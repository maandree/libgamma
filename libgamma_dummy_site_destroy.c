/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_dummy_site_destroy(libgamma_site_state_t *restrict this)
{
	libgamma_dummy_site_t *data = this->data;
	if (data) {
		free(data->partitions);
		free(data);
	}
}
