/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps16_destroy(struct libgamma_gamma_ramps16 *restrict this)
{
	free(this->red);
}
