/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Return the capabilities of an adjustment method
 * 
 * @param   this    The data structure to fill with the method's capabilities
 * @param   method  The adjustment method (display server and protocol)
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int
libgamma_method_capabilities(libgamma_method_capabilities_t *restrict this, int method)
{
	memset(this, 0, sizeof(*this));

	switch (method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		libgamma_##CNAME##_method_capabilities(this);\
		return 0;
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
