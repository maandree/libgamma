/* See LICENSE file for copyright and license details. */
#include "common.h"


#define X(...) +1

/**
 * The number of values defined in `libgamma_connector_type_t`
 * in the version of the library the program is linked against
 */
const int libgamma_connector_type_count = LIST_CONNECTOR_TYPES(X);

#undef X
