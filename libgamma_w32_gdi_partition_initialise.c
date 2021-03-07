/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
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
libgamma_w32_gdi_partition_initialise(struct libgamma_partition_state *restrict this,
                                      struct libgamma_site_state *restrict site, size_t partition)
{
	DWORD n = 0;
	DISPLAY_DEVICE display;

	(void) site;

	if (partition)
		return LIBGAMMA_NO_SUCH_PARTITION;

	/* Count CRTC:s by iteration over all possible identifiers
	   until we reach on that does not exist */
	display.cb = sizeof(DISPLAY_DEVICE);
	while (EnumDisplayDevices(NULL, n, &display, 0))
		if (n++ == UINT32_MAX)
			return LIBGAMMA_IMPOSSIBLE_AMOUNT;
	this->crtcs_available = (size_t)n;
	return 0;
}
