/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the name of the constant for a subpixel order,
 * for example "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 * for `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB`
 * 
 * @param   order  The subpixel order
 * @return         The name of the constant for subpixel order,
 *                 `NULL` if not recognised (errno is not changed)
 */
const char *
libgamma_const_of_subpixel_order(enum libgamma_subpixel_order order)
{
	switch ((int)order) {
#define X(CONST, ...)\
	case CONST:\
		return #CONST;
	LIST_SUBPIXEL_ORDERS(X)
#undef X
	default:
		return NULL;
	}
}
