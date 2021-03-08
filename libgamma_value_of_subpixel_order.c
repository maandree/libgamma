/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the value of a subpixel order
 * 
 * @param   order  The name of the subpixel order, for example
 *                 "Horizontal RGB" or "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB"
 * @param   out    Output parameter for the subpixel order, only set on success;
 *                 for example `LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB` for
 *                 "Horizontal RGB" and "LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB";
 * @return         Zero on success, `LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED` of not defined
 */
int
libgamma_value_of_subpixel_order(const char *order, enum libgamma_subpixel_order *out)
{
#define X(CONST, NAME, ...)\
	if (!strcmp(order, NAME))\
		goto case_##CONST;
	LIST_SUBPIXEL_ORDERS(X)
#undef X

#define X(CONST, ...)\
	if (!strcmp(order, #CONST)) {\
	case_##CONST:\
		*out = CONST;\
		return 0;\
	}
	LIST_SUBPIXEL_ORDERS(X)
#undef X

	return LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED;
}
