#include "common.h"


/**
 * Check whether an adjustment method is available,
 * non-existing (invalid) methods will be identified
 * as not available under the rationale that the
 * library may be out of date
 * 
 * @param   method  The adjustment method
 * @return          Whether the adjustment method is available
 */
int
libgamma_is_method_available(int method)
{
	switch (method) {
#define X(CONST, NAME, CNAME, ENABLED)\
	case CONST:\
		return ENABLED;
	LIST_METHODS(X)
#undef X
	default:
		return 0;
	}
}
