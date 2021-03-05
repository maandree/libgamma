/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the value of a `libgamma` error definition refered to by name
 * 
 * @param   name  The name of the definition associated with the error code
 * @return        The error code, zero if the name is `NULL`
 *                or does not refer to a `libgamma` error
 */
int
libgamma_value_of_error(const char *name)
{
	if (!name)
		return 0;
#define X(NAME, ...)\
	if (!strcmp(name, #NAME))\
		return NAME;
	LIST_ERRORS(X)
	#undef X
	return 0;
}
