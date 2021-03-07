/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
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
libgamma_x_vidmode_partition_initialise(struct libgamma_partition_state *restrict this,
                                        struct libgamma_site_state *restrict site, size_t partition)
{
	this->crtcs_available = 1;
	return partition < site->partitions_available ? 0 : LIBGAMMA_NO_SUCH_PARTITION;
}
