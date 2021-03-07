/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_dummy_partition_initialise(struct libgamma_partition_state *restrict this,
                                    struct libgamma_site_state *restrict site, size_t partition)
{
	struct libgamma_crtc_information template = libgamma_dummy_internal_configurations.crtc_info_template;
	struct libgamma_dummy_site *site_data = site->data;
	struct libgamma_dummy_partition *data = &site_data->partitions[partition];
	struct libgamma_dummy_crtc *crtc_data;
	size_t i, n;

	this->data = NULL;

	if (partition >= site_data->partition_count)
		return LIBGAMMA_NO_SUCH_PARTITION;

	this->data = data;
	data->state = this;

	data->crtcs = calloc(data->crtc_count, sizeof(*data->crtcs));
	if (!data->crtcs)
		goto fail;
	for (i = 0; i < data->crtc_count; i++) {
		crtc_data = data->crtcs + i;
		crtc_data->info = template;

		/* Duplicate strings */
		if (crtc_data->info.edid) {
			crtc_data->info.edid = malloc(crtc_data->info.edid_length * sizeof(char));
			if (!crtc_data->info.edid)
				goto fail;
			memcpy(crtc_data->info.edid, template.edid, crtc_data->info.edid_length * sizeof(char));
		}
		if (crtc_data->info.connector_name) {
			n = strlen(crtc_data->info.connector_name) + 1;
			crtc_data->info.connector_name = malloc(n * sizeof(char));
			if (!crtc_data->info.connector_name)
				goto fail;
			memcpy(crtc_data->info.connector_name, template.connector_name, n * sizeof(char));
		}
	}

	this->crtcs_available = data->crtc_count;

	return 0;

fail:
	for (i = 0; i < data->crtc_count; i++) {
		free(data->crtcs[i].info.edid);
		free(data->crtcs[i].info.connector_name);
	}
	free(data->crtcs);
	data->crtcs = NULL;
	return LIBGAMMA_ERRNO_SET;
}
