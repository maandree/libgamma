/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the value of a subpixel order
 * 
 * @param   order  The name of the subpixel order, for example
 *                 "Horizontal RGB" or "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 * @return         The subpixel order; for example `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB`
 *                 for "Horizontal RGB" and "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 */
int
libgamma_value_of_subpixel_order(const char *order)
{
#define X(CONST, NAME, ...)\
	if (!strcmp(order, NAME))\
		return CONST;
	LIST_SUBPIXEL_ORDERS(X)
#undef X

#define X(CONST, ...)\
	if (!strcmp(order, #CONST))\
		return CONST;
	LIST_SUBPIXEL_ORDERS(X)
#undef X

	return 0;
}
