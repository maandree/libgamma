/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void
libgamma_dummy_site_destroy(struct libgamma_site_state *restrict this)
{
	struct libgamma_dummy_site *data = this->data;
	if (data) {
		free(data->partitions);
		free(data);
	}
}
