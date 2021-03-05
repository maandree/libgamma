/* See LICENSE file for copyright and license details. */
#include "common.h"


#define X(...) -1

/**
 * The number of the libgamma error with the
 * lowest number in the version of the library
 * that the program is linked against
 */
const int libgamma_error_min = LIST_ERRORS(X);

#undef X
