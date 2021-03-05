/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get a description of an error
 * 
 * @param   error_code  The error code, may be an `errno` value, if
 *                      `LIBGAMMA_ERRNO_SET`, the current value of `errno`
 *                      will be used
 * @return              The description associated with the error code,
 *                      `NULL` if the error code is not recognised
 */
const char *
libgamma_strerror(int error_code)
{
	const char *desc;
	if (error_code == LIBGAMMA_ERRNO_SET)
		error_code = errno;
	switch (error_code) {
#define X(NAME, DESC)\
	case NAME:\
		desc = DESC;\
		break;
	LIST_ERRORS(X)
#undef X
	default:
		desc = NULL;
		break;
	}
	return desc ? desc : error_code < 0 ? NULL : strerror(error_code);
}
