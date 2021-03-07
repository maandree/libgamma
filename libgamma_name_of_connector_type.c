/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the name of a connector type,
 * for example "VGA" for `LIBGAMMA_CONNECTOR_TYPE_VGA`
 * 
 * "Unknown" is returned for `LIBGAMMA_CONNECTOR_TYPE_Unknown`,
 * "TV"      is returned for `LIBGAMMA_CONNECTOR_TYPE_TV`,
 * "Virtual" is returned for `LIBGAMMA_CONNECTOR_TYPE_Virtual`
 * 
 * @param   connector  The connector type
 * @return             The name connector type, `NULL` if not
 *                     recognised (errno is not changed)
 */
const char *
libgamma_name_of_connector_type(enum libgamma_connector_type connector)
{
	switch ((int)connector) {
#define X(CONST, NAME, ...)\
	case CONST:\
		return NAME;
	LIST_CONNECTOR_TYPES(X)
#undef X
	default:
		return NULL;
	}
}
