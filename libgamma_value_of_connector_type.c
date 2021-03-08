/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the value of a connector type
 * 
 * @param   connector  The name of the connector type, for example
 *                     "VGA" or "LIBGAMMA_CONNECTOR_TYPE_VGA"
 * @param   out        Output parameter for the connector type, only set on success;
 *                     for example `LIBGAMMA_CONNECTOR_TYPE_VGA` for "VGA" and
 *                     "LIBGAMMA_CONNECTOR_TYPE_VGA";
 * @return             Zero on success, `LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED` of not defined
 */
int
libgamma_value_of_connector_type(const char *connector, enum libgamma_connector_type *out)
{
#define X(CONST, NAME, ...)\
	if (!strcmp(connector, NAME))\
		goto case_##CONST;
	LIST_CONNECTOR_TYPES(X)
#undef X

#define X(CONST, ...)\
	if (!strcmp(connector, #CONST)) {\
	case_##CONST:\
		*out = CONST;\
		return 0;\
	}
	LIST_CONNECTOR_TYPES(X)
#undef X

	return LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED;
}
