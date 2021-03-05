/* See LICENSE file for copyright and license details. */
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
libgamma_quartz_cg_partition_initialise(libgamma_partition_state_t *restrict this,
                                       libgamma_site_state_t *restrict site, size_t partition)
{
	CGDirectDisplayID *crtcs, *crtcs_old;
	uint32_t cap = 4, n;

	(void) site;

	this->data = NULL;

	if (partition)
		return LIBGAMMA_NO_SUCH_PARTITION;

	/* Allocate array of CRTC ID:s */
	crtcs = malloc((size_t)cap * sizeof(CGDirectDisplayID));
	if (!crtcs)
		return LIBGAMMA_ERRNO_SET;

	/* It is not possible to ask CoreGraphics how many CRTC:s are
	 * available. We have to ask it to give us a ID:s of a number
	 * of CRTC:s and ask for more if we got as many as we asked for. */
	for (;;) {
		/* Ask for CRTC ID:s */
		if (CGGetOnlineDisplayList(cap, crtcs, &n) != kCGErrorSuccess)
			return free(crtcs), LIBGAMMA_LIST_CRTCS_FAILED;
		/* If we did not get as many as we asked for then we have all */
		if (n < cap)
			break;
		/* Increase the number CRTC ID:s to ask for */
		if (cap > SIZE_MAX / 2) /* We could also test ~0, but it is still too many */
			return free(crtcs), LIBGAMMA_IMPOSSIBLE_AMOUNT;
		cap <<= 1;
		/* Grow the array of CRTC ID:s so that it can fit all we are asking for */
		crtcs = realloc(crtcs_old = crtcs, (size_t)cap * sizeof(CGDirectDisplayID));
		if (!crtcs) {
			free(crtcs_old);
			return LIBGAMMA_ERRNO_SET;
		}
	}

	/* Store CRTC ID:s and CRTC count */
	this->data = crtcs;
	this->crtcs_available = (size_t)n;
	return 0;
}
