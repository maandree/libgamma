/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
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
libgamma_w32_gdi_site_initialise(struct libgamma_site_state *restrict this, char *restrict site)
{
	this->partitions_available = 1;
	return !site ? 0 : LIBGAMMA_NO_SUCH_SITE;
}
