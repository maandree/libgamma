#include "common.h"


/**
 * Get the name of an adjustment method,
 * for example "randr" for `LIBGAMMA_METHOD_X_RANDR`
 * 
 * @param   method  The adjustment method
 * @return          The name adjustment method, `NULL` if not
 *                  recognised (errno is not changed)
 */
const char *
libgamma_name_of_method(int method)
{
	switch (method) {
#define X(CONST, NAME, CNAME, ENABLED)\
	case CONST: return #NAME;
	LIST_METHODS(X)
#undef X
	default:
		return NULL;
	}
}
