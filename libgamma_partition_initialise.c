/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int
libgamma_partition_initialise(libgamma_partition_state_t *restrict this, libgamma_site_state_t *restrict site, size_t partition)
{
	this->site = site;
	this->partition = partition;

	switch (site->method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		return libgamma_##CNAME##_partition_initialise(this, site, partition);
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
