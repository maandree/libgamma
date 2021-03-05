/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
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
libgamma_x_randr_site_initialise(libgamma_site_state_t *restrict this, char *restrict site)
{
	xcb_generic_error_t *error = NULL;
	xcb_connection_t *restrict connection;
	xcb_randr_query_version_cookie_t cookie;
	xcb_randr_query_version_reply_t *restrict reply;
	const xcb_setup_t *restrict setup;
	xcb_screen_iterator_t iter;

	/* Connect to the display server */
	this->data = connection = xcb_connect(site, NULL);
	if (!connection || xcb_connection_has_error(connection))
		return LIBGAMMA_OPEN_SITE_FAILED;

	/* Query the version of the X RandR extension protocol */
	cookie = xcb_randr_query_version(connection, RANDR_VERSION_MAJOR, RANDR_VERSION_MINOR);
	reply = xcb_randr_query_version_reply(connection, cookie, &error);

	/* Check for version query failure */
	if (error || !reply) {
		/* Release resources */
		free(reply);
		/* If `xcb_connect` failed, both `error` and `reply` will be `NULL`.
		 * TODO: Can both be `NULL` for any other reason? */
		if (!error && !reply)
			return LIBGAMMA_OPEN_SITE_FAILED;
		xcb_disconnect(connection);
		/* Translate and report error. */
		if (error != NULL)
			return libgamma_x_randr_internal_translate_error(error->error_code, LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED, 0);
		return LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED;
	}

	/* Check protocol compatibility,
	   we require 1.3 but 2.x may not be backwards compatible */
	if (reply->major_version != RANDR_VERSION_MAJOR || reply->minor_version < RANDR_VERSION_MINOR) {
#ifdef DEBUG
		/* Print used protocol */
		fprintf(stderr, "libgamma: RandR protocol version: %u.%u", reply->major_version, reply->minor_version);
#endif
		/* Release resources */
		free(reply);
		xcb_disconnect(connection);
		/* Report error */
		return LIBGAMMA_PROTOCOL_VERSION_NOT_SUPPORTED;
	}

	/* We do not longer need to know the version of the protocol */
	free(reply);

	/* Get available screens */
	setup = xcb_get_setup(connection);
	if (!setup) {
		xcb_disconnect(connection);
		return LIBGAMMA_LIST_PARTITIONS_FAILED;
	}
	iter = xcb_setup_roots_iterator(setup);
	/* Get the number of available screens */
	this->partitions_available = (size_t)iter.rem;

	/* Sanity check the number of available screens. */
	return iter.rem >= 0 ? 0 : LIBGAMMA_NEGATIVE_PARTITION_COUNT;
}
