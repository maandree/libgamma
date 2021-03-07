/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
#include "common.h"


/**
 * Duplicate a memory area
 * 
 * @param   ptr    The memory area
 * @param   nelem  The number of elements in the memory area
 * @param   size   The size, in bytes, of each element
 * @return         A duplication of the memory, `NULL` if zero-length or on error
 */
static inline void *
duparray(void *restrict ptr, size_t nelem, size_t size)
{
	char *restrict rc;
	size_t bytes;
	if (nelem > SIZE_MAX / size) {
		errno = ENOMEM;
		return NULL;
	}
	bytes = nelem * size;
	if (!bytes)
		return NULL;
	rc = malloc(bytes);
	if (!rc)
		return NULL;
	memcpy(rc, ptr, bytes);
	return rc;
}


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to.
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_x_randr_partition_initialise(struct libgamma_partition_state *restrict this,
                                      struct libgamma_site_state *restrict site, size_t partition)
{
	int fail_rc = LIBGAMMA_ERRNO_SET;
	xcb_connection_t *restrict connection = site->data;
	xcb_screen_t *restrict screen = NULL;
	xcb_generic_error_t *error = NULL;
	const xcb_setup_t *restrict setup;
	xcb_screen_iterator_t iter;
	xcb_randr_get_screen_resources_current_cookie_t cookie;
	xcb_randr_get_screen_resources_current_reply_t *restrict reply;
	xcb_randr_crtc_t *restrict crtcs;
	xcb_randr_output_t *restrict outputs;
	struct libgamma_x_randr_partition_data *restrict data;
	xcb_randr_get_output_info_cookie_t out_cookie;
	xcb_randr_get_output_info_reply_t *out_reply;
	size_t i;
	uint16_t j;

	/* Get screen list */
	setup = xcb_get_setup(connection);
	if (!setup)
		return LIBGAMMA_LIST_PARTITIONS_FAILED;
	iter = xcb_setup_roots_iterator(setup);

	/* Get the screen */
	for (i = 0; iter.rem > 0; i++, xcb_screen_next(&iter))
		if (i == partition) {
			screen = iter.data;
			break;
		}
	/* Report failure if we did not find the screen */
	if (!iter.rem)
		return LIBGAMMA_NO_SUCH_PARTITION;

	/* Check that the screen is not `NULL`.
	 * (Do not think this can happen, but why not.) */
	if (!screen)
		return LIBGAMMA_NULL_PARTITION;

	/* Get the current resources of the screen */
	cookie = xcb_randr_get_screen_resources_current(connection, screen->root);
	reply = xcb_randr_get_screen_resources_current_reply(connection, cookie, &error);
	if (error)
		return libgamma_x_randr_internal_translate_error(error->error_code, LIBGAMMA_LIST_CRTCS_FAILED, 0);

	/* Get the number of available CRTC:s */
	this->crtcs_available = reply->num_crtcs;
	/* Get the CRTC and output lists */
	crtcs = xcb_randr_get_screen_resources_current_crtcs(reply);
	outputs = xcb_randr_get_screen_resources_current_outputs(reply);
	if (!crtcs || !outputs) {
		free(reply);
		return LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED;
	}

	/* Allocate adjustment method dependent data memory area.
	 * We use `calloc` because we want `data`'s pointers to be `NULL` if not allocated at `fail`. */
	data = calloc(1, sizeof(*data));
	if (!data)
		goto fail;

	/* Copy the CRTC:s, just so we do not have to keep the reply in memory */
	data->crtcs = duparray(crtcs, (size_t)reply->num_crtcs, sizeof(*crtcs));
	if (!data->crtcs && reply->num_crtcs > 0)
		goto fail;

	/* Copy the outputs as well */
	data->outputs = duparray(outputs, (size_t)reply->num_outputs, sizeof(*outputs));
	if (!data->outputs && reply->num_outputs > 0)
		goto fail;

	/* Get the number of available outputs */
	data->outputs_count = (size_t)reply->num_outputs;

	/* Create mapping table from CRTC indices to output indicies. (injection) */
	if (reply->num_crtcs) {
		if (reply->num_crtcs > SIZE_MAX / sizeof(*data->crtc_to_output)) {
			errno = ENOMEM;
			goto fail;
		}
		data->crtc_to_output = malloc((size_t)reply->num_crtcs * sizeof(*data->crtc_to_output));
		if (!data->crtc_to_output)
			goto fail;
	}
	/* All CRTC:s should be mapped, but incase they are not, all unmapped CRTC:s should have
	 * an invalid target, namely `SIZE_MAX`, which is 1 more than the theoretical limit */
	for (i = 0; i < (size_t)reply->num_crtcs; i++)
		data->crtc_to_output[i] = SIZE_MAX;
	/* Fill the table */
	for (i = 0; i < (size_t)reply->num_outputs; i++) {
		/* Query output (target) information */
		out_cookie = xcb_randr_get_output_info(connection, outputs[i], reply->config_timestamp);
		out_reply = xcb_randr_get_output_info_reply(connection, out_cookie, &error);
		if (error) {
			fail_rc = libgamma_x_randr_internal_translate_error(error->error_code,
			                                                    LIBGAMMA_OUTPUT_INFORMATION_QUERY_FAILED, 0);
			goto fail;
		}

		/* Find CRTC (source) */
		for (j = 0; j < reply->num_crtcs; j++) {
			if (crtcs[j] == out_reply->crtc) {
				data->crtc_to_output[j] = i;
				break;
			}
		}

		/* Release output information */
		free(out_reply);
	}

	/* Store the configuration timestamp */
	data->config_timestamp = reply->config_timestamp;
	/* Store the adjustment method dependent data */
	this->data = data;
	/* Release resources and return successfully */
	free(reply);
	return 0;

fail:
	/* Release resources and return with an error */
	if (data) {
		free(data->crtcs);
		free(data->outputs);
		free(data->crtc_to_output);
		free(data);
	}
	free(reply);
	return fail_rc;
}
