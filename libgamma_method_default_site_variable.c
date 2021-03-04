/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Return the default variable that determines
 * the default site for an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The environ variables that is used to determine the
 *                  default site, `NULL` if there is none, that is, if
 *                  the method does not support multiple sites
 */
const char *
libgamma_method_default_site_variable(int method)
{
	switch (method) {
	case LIBGAMMA_METHOD_X_RANDR:
		return "DISPLAY";
	case LIBGAMMA_METHOD_X_VIDMODE:
		return "DISPLAY";
	default:
		return NULL;
	}
}
