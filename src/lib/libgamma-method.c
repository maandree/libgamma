/* See LICENSE file for copyright and license details. */
#include "libgamma-method.h"


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>



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
libgamma_gamma_ramps8_initialise(libgamma_gamma_ramps8_t *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
	this->red   = malloc(n * sizeof(uint8_t));
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps8_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps8_destroy(libgamma_gamma_ramps8_t *restrict this)
{
	free(this->red);
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps8_initialise` or otherwise
 * initialises in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps8_free(libgamma_gamma_ramps8_t *restrict this)
{
	free(this->red);
	free(this);
}



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
libgamma_gamma_ramps16_initialise(libgamma_gamma_ramps16_t *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
#ifdef HAVE_LIBGAMMA_METHOD_LINUX_DRM
	/* Valgrind complains about us reading uninitialize memory if we just use malloc */
	this->red = calloc(n, sizeof(uint16_t));
#else
	this->red = malloc(n * sizeof(uint16_t));
#endif
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps16_destroy(libgamma_gamma_ramps16_t *restrict this)
{
	free(this->red);
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps_initialise` or otherwise
 * initialises in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps16_free(libgamma_gamma_ramps16_t *restrict this)
{
	free(this->red);
	free(this);
}



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
libgamma_gamma_ramps32_initialise(libgamma_gamma_ramps32_t *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
	this->red   = malloc(n * sizeof(uint32_t));
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps32_destroy(libgamma_gamma_ramps32_t *restrict this)
{
	free(this->red);
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialises in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps32_free(libgamma_gamma_ramps32_t *restrict this)
{
	free(this->red);
	free(this);
}



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
libgamma_gamma_ramps64_initialise(libgamma_gamma_ramps64_t *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
	this->red   = malloc(n * sizeof(uint64_t));
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps64_destroy(libgamma_gamma_ramps64_t *restrict this)
{
	free(this->red);
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialises in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_ramps64_free(libgamma_gamma_ramps64_t *restrict this)
{
	free(this->red);
	free(this);
}



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
libgamma_gamma_rampsf_initialise(libgamma_gamma_rampsf_t *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
	this->red   = malloc(n * sizeof(float));
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_rampsf_destroy(libgamma_gamma_rampsf_t *restrict this)
{
	free(this->red);
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialises in the proper manner, as well as release the pointer
 * to the structure
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_rampsf_free(libgamma_gamma_rampsf_t *restrict this)
{
	free(this->red);
	free(this);
}



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
libgamma_gamma_rampsd_initialise(libgamma_gamma_rampsd_t *restrict this)
{
	size_t n = this->red_size + this->green_size + this->blue_size;
	this->red   = malloc(n * sizeof(double));
	this->green = &this->  red[this->  red_size];
	this->blue  = &this->green[this->green_size];
	return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void
libgamma_gamma_rampsd_destroy(libgamma_gamma_rampsd_t *restrict this)
{
	free(this->red);
}


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
