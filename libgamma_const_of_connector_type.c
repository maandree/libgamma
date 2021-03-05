/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the name of the constant for a connector
 * type, for example "LIBGAMMA_CONNECTOR_TYPE_VGA"
 * for `LIBGAMMA_CONNECTOR_TYPE_VGA`
 * 
 * @param   connector  The connector type
 * @return             The name of the constant for connector type,
 *                     `NULL` if not recognised (errno is not changed)
 */
const char *
libgamma_const_of_connector_type(int connector)
{
	switch (connector) {
#define X(CONST, ...)\
	case CONST:\
		return #CONST;
	LIST_CONNECTOR_TYPES(X)
#undef X
	default:
		return NULL;
	}
}
