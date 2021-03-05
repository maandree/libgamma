#include "common.h"


/**
 * Get the name of the constant for an adjustment
 * method, for example "LIBGAMMA_METHOD_X_RANDR"
 * for `LIBGAMMA_METHOD_X_RANDR`
 * 
 * @param   method  The adjustment method
 * @return          The name of the constant for adjustment method,
 *                  `NULL` if not recognised (errno is not changed)
 */
const char *
libgamma_const_of_method(int method)
{
	switch (method) {
#define X(CONST, ...)\
	case CONST:\
		return #CONST;
	LIST_METHODS(X)
#undef X
	default:
		return NULL;
	}
}
