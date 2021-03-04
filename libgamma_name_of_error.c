/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the name of the definition associated with a `libgamma` error code
 * 
 * @param   value  The error code
 * @return         The name of the definition associated with the error code,
 *                 `NULL` if the error code does not exist
 */
const char *
libgamma_name_of_error(int value)
{
	switch (value) {
#define X(NAME, DESC)\
	case NAME: return #NAME;
	LIST_ERRORS(X)
#undef X
	default:
		return NULL;
	}
}
