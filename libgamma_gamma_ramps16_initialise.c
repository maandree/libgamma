/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size`, and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
int
libgamma_gamma_ramps16_initialise(struct libgamma_gamma_ramps16 *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
#ifdef HAVE_LIBGAMMA_METHOD_LINUX_DRM
	/* Valgrind complains about us reading uninitialize memory if we just use malloc */
	this->red = calloc(n, sizeof(*this->red));
#else
	this->red = malloc(n * sizeof(*this->red));
#endif
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}
