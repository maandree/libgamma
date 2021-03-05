/* See LICENSE file for copyright and license details. */
#include "common.h"


#define X(...) +1

/**
 * The number of values defined in `libgamma_subpixel_order_t`
 * in the version of the library the program is linked against
 */
const int libgamma_subpixel_order_count = LIST_SUBPIXEL_ORDERS(X);

#undef X
