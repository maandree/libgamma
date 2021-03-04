/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Return the default site for an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The default site, `NULL` if it cannot be determined or
 *                  if multiple sites are not supported by the adjustment
 *                  method
 */
const char *
libgamma_method_default_site(int method)
{
	const char *restrict var = libgamma_method_default_site_variable(method);
	const char *restrict env;

	/* Return `NULL` if there is no variable to read */
	if (!var)
		return NULL;

	/* Read the variable */
	env = getenv(var);
	/* Return `NULL` if it does not exist (or is empty) */
	if (!env || !*env)
		return NULL;

	/* Return the variable's value */
	return env;
}
