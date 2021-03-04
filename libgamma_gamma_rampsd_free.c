/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialises in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_rampsd_free(libgamma_gamma_rampsd_t *restrict this)
{
	free(this->red);
	free(this);
}
