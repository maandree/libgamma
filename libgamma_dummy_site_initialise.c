/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. Once the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int
libgamma_dummy_site_initialise(struct libgamma_site_state *restrict this, char *restrict site)
{
	struct libgamma_dummy_site *data = NULL;
	size_t i, sites, crtcs;

	sites = libgamma_dummy_internal_configurations.site_count;
	if (!libgamma_dummy_internal_configurations.capabilities.multiple_sites)
		sites = !!sites;
	this->data = NULL;

	if (site && *site && (atoll(site) < 0 || sites <= (size_t)atoll(site)))
		return LIBGAMMA_NO_SUCH_SITE;

	data = malloc(sizeof(*data));
	if (!data)
		goto fail;

	this->data = data;
	data->state = this;

	data->partition_count = libgamma_dummy_internal_configurations.default_partition_count;
	if (!libgamma_dummy_internal_configurations.capabilities.multiple_partitions)
		data->partition_count = !!data->partition_count;

	crtcs = libgamma_dummy_internal_configurations.default_crtc_count;
	if (!libgamma_dummy_internal_configurations.capabilities.multiple_crtcs)
		crtcs = !!crtcs;

	if (data->partition_count * sizeof(*data->partitions)) {
		errno = ENOMEM;
		goto fail;
	}
	data->partitions = malloc(data->partition_count * sizeof(*data->partitions));
	if (!data->partitions)
		goto fail;

	for (i = 0; i < data->partition_count; i++)
		data->partitions[i].crtc_count = crtcs;

	this->partitions_available = data->partition_count;

	return 0;

fail:
	free(data);
	this->data = NULL;
	return LIBGAMMA_ERRNO_SET;
}
