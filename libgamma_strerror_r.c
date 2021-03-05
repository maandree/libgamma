/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get a description of an error
 * 
 * @param   error_code  The error code, may be an `errno` value, if
 *                      `LIBGAMMA_ERRNO_SET`, the current value of `errno`
 *                      will be used
 * @param   buf         Buffer that shall be used if a description must be generated
 * @param   bufsize     The size of `buf`, 1024 is recommended
 * @return              The description associated with the error code;
 *                      can only be `NULL` if `buf` is `NULL`. If the buffer
 *                      is insufficient, a truncated but NUL-terminated
 *                      description is returned and `errno` is set to `ERANGE`;
 *                      `errno` is otherwise unmodified
 */
const char *
libgamma_strerror_r(int error_code, char buf[], size_t bufsize)
{
	const char *desc;
	int saved_errno;
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
	if (desc)
		return desc;
	if (error_code >= 0) {
		saved_errno = errno;
		desc = _Generic(strerror_r(error_code, buf, bufsize),
		                /* XSI strerror_r */
		                int: (errno = (int)(intptr_t)strerror_r(error_code, buf, bufsize)) ? NULL : buf,
		                /* GNU strerror_r */
		                char *: (char *)(intptr_t)strerror_r(error_code, buf, bufsize));
		if (desc) {
			errno = saved_errno;
			if (!buf || strcmp(buf, "No error information"))
				return buf;
		} else if (errno == ERANGE) {
			return buf;
		} else {
			errno = saved_errno;
		}
	}
	if (!buf)
		return buf;
	*buf = '\0'; /* TODO what happended here? */
	snprintf(buf, bufsize, "Unknown error #%i\n", error_code);
	return buf;
}
