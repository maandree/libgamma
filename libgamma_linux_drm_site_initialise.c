/* See LICENSE file for copyright and license details. */
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
libgamma_linux_drm_site_initialise(libgamma_site_state_t *restrict this, char *restrict site)
{
	char pathname[PATH_MAX];
	struct stat _attr;
  
	if (site)
		return LIBGAMMA_NO_SUCH_SITE;

	/* Count the number of available graphics cards by
	   `stat`:ing their existence in an API filesystem */
	this->partitions_available = 0;
	for (;;) {
		/* Construct pathname of graphics card device */
		snprintf(pathname, sizeof(pathname) / sizeof(char),
		          DRM_DEV_NAME, DRM_DIR_NAME, (int)(this->partitions_available));
		/* `stat` the graphics card's existence */
		if (stat(pathname, &_attr))
			break;
		/* Move on to next graphics card */
		if (this->partitions_available++ > INT_MAX)
			return LIBGAMMA_IMPOSSIBLE_AMOUNT;
	}
	return 0;
}
