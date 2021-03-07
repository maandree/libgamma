/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
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
libgamma_x_vidmode_site_initialise(struct libgamma_site_state *restrict this, char *restrict site)
{
	/* Connect to the display */
	Display *restrict connection;
	int _major, _minor, screens;
	this->data = connection = XOpenDisplay(site);
	if (!this->data)
		return LIBGAMMA_OPEN_SITE_FAILED;
	/* Query X VidMode extension protocol version */
	if (!XF86VidModeQueryVersion(connection, &_major, &_minor)) {
		XCloseDisplay(connection);
		return LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED;
	}
	/* Query the number of available screens */
	screens = ScreenCount(connection);
	if (screens < 0) {
		XCloseDisplay(connection);
		return LIBGAMMA_NEGATIVE_PARTITION_COUNT;
	}
	this->partitions_available = (size_t)screens;
	return 0;
}
