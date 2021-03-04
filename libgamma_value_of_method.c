#include "common.h"


/**
 * Get the value of an adjustment method
 * 
 * @param   method  The name of the adjustment method, for example
 *                  "randr" or "LIBGAMMA_METHOD_X_RANDR"
 * @return          The adjustment method; for example `LIBGAMMA_METHOD_X_RANDR`
 *                  for "randr" and "LIBGAMMA_METHOD_X_RANDR"
 */
int
libgamma_value_of_method(const char *method)
{
#define X(CONST, NAME, CNAME, ENABLED)\
	if (!strcmp(method, #NAME))\
		return CONST;
	LIST_METHODS(X)
#undef X

#define X(CONST, NAME, CNAME, ENABLED)\
	if (!strcmp(method, #CONST))\
		return CONST;
	LIST_METHODS(X)
#undef X

	return 0;
}
