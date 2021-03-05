/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the name of a subpixel order, for example
 * "Horizontal RGB" for `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB`
 * 
 * "Unknown" is returned for `LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN`,
 * "None"    is returned for `LIBGAMMA_SUBPIXEL_ORDER_NONE`
 * 
 * @param   order  The subpixel order
 * @return         The name subpixel order, `NULL` if not
 *                 recognised (errno is not changed)
 */
const char *
libgamma_name_of_subpixel_order(int order)
{
	switch (order) {
#define X(CONST, NAME, ...)\
	case CONST:\
		return NAME;
	LIST_SUBPIXEL_ORDERS(X)
#undef X
	default:
		return NULL;
	}
}
