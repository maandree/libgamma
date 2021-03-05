/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
#include "common.h"


/**
 * Translate an xcb error into a libgamma error
 * 
 * @param   error_code     The xcb error
 * @param   default_error  The libgamma error to use if the xcb error is not recognised
 * @param   return_errno   Whether an `errno` value may be returned
 * @return                 The libgamma error
 */
int
libgamma_x_randr_internal_translate_error(int error_code, int default_error, int return_errno)
{
	int r = LIBGAMMA_ERRNO_SET;
	switch (error_code) {
	case XCB_CONN_ERROR:                    errno = ECONNABORTED;  break;
	case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:  errno = ENOPROTOOPT;  break;
	case XCB_CONN_CLOSED_MEM_INSUFFICIENT:  errno = ENOMEM;  break;
	case XCB_CONN_CLOSED_REQ_LEN_EXCEED:    errno = EMSGSIZE;  break;
	case XCB_CONN_CLOSED_PARSE_ERR:         r = LIBGAMMA_NO_SUCH_SITE;  break;
	case XCB_CONN_CLOSED_INVALID_SCREEN:    r = LIBGAMMA_NO_SUCH_PARTITION;  break;
	case XCB_CONN_CLOSED_FDPASSING_FAILED:  errno = EIO;  break;
	default:
		r = default_error;
		break;
	}
	return (return_errno && r > 0) ? errno : r;
}
