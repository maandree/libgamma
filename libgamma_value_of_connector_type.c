/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the value of a connector type
 * 
 * @param   connector  The name of the connector type, for example
 *                     "VGA" or "LIBGAMMA_CONNECTOR_TYPE_VGA"
 * @return             The connector type; for example `LIBGAMMA_CONNECTOR_TYPE_VGA`
 *                     for "VGA" and "LIBGAMMA_CONNECTOR_TYPE_VGA"
 */
int
libgamma_value_of_connector_type(const char *connector)
{
#define X(CONST, NAME, ...)\
	if (!strcmp(connector, NAME))\
		return CONST;
	LIST_CONNECTOR_TYPES(X)
#undef X

#define X(CONST, ...)\
	if (!strcmp(connector, #CONST))\
		return CONST;
	LIST_CONNECTOR_TYPES(X)
#undef X

	return 0;
}
