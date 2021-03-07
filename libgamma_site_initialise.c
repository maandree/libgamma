/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   method  The adjustment method (display server and protocol)
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. Once the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocated on the stack. Note however that it will
 *                  not be `free`:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int
libgamma_site_initialise(struct libgamma_site_state *restrict this, int method, char *restrict site)
{
	this->method = method;
	this->site = site;

	switch (method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		return libgamma_##CNAME##_site_initialise(this, site);
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
