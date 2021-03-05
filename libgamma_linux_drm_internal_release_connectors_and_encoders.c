/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release all connectors and encoders
 * 
 * @param  this  The graphics card data
 */
void
libgamma_linux_drm_internal_release_connectors_and_encoders(libgamma_drm_card_data_t *restrict this)
{
	size_t i, n;

	/* Release individual encoders */
	if (this->encoders)
		for (i = 0, n = (size_t)(this->res->count_connectors); i < n; i++)
			if (this->encoders[i])
				drmModeFreeEncoder(this->encoders[i]);
	/* Release encoder array */
	free(this->encoders);
	this->encoders = NULL;

	/* Release individual connectors */
	if (this->connectors)
		for (i = 0, n = (size_t)(this->res->count_connectors); i < n; i++)
			if (this->connectors[i])
				drmModeFreeConnector(this->connectors[i]);
	/* Release connector array */
	free(this->connectors);
	this->connectors = NULL;
}
