/**
 * libgamma — Display server abstraction layer for gamma ramp adjustments
 * Copyright © 2014  Mattias Andrée (maandree@member.fsf.org)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "libgamma-method.h"


#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
int libgamma_gamma_ramps_initialise(libgamma_gamma_ramps_t* restrict this)
{
  size_t n = this->red_size + this->green_size + this->blue_size;
  this->red   = malloc(n * sizeof(uint16_t));
  this->green = this->  red + this->  red_size;
  this->blue  = this->green + this->green_size;
  return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void libgamma_gamma_ramps_destroy(libgamma_gamma_ramps_t* restrict this)
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
void libgamma_gamma_ramps_free(libgamma_gamma_ramps_t* restrict this)
{
  free(this->red);
  free(this);
}



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
int libgamma_gamma_ramps32_initialise(libgamma_gamma_ramps32_t* restrict this)
{
  size_t n = this->red_size + this->green_size + this->blue_size;
  this->red   = malloc(n * sizeof(uint32_t));
  this->green = this->  red + this->  red_size;
  this->blue  = this->green + this->green_size;
  return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps32_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void libgamma_gamma_ramps32_destroy(libgamma_gamma_ramps32_t* restrict this)
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
void libgamma_gamma_ramps32_free(libgamma_gamma_ramps32_t* restrict this)
{
  free(this->red);
  free(this);
}



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
int libgamma_gamma_ramps64_initialise(libgamma_gamma_ramps64_t* restrict this)
{
  size_t n = this->red_size + this->green_size + this->blue_size;
  this->red   = malloc(n * sizeof(uint64_t));
  this->green = this->  red + this->  red_size;
  this->blue  = this->green + this->green_size;
  return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_ramps64_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void libgamma_gamma_ramps64_destroy(libgamma_gamma_ramps64_t* restrict this)
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
void libgamma_gamma_ramps64_free(libgamma_gamma_ramps64_t* restrict this)
{
  free(this->red);
  free(this);
}



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
int libgamma_gamma_rampsf_initialise(libgamma_gamma_rampsf_t* restrict this)
{
  size_t n = this->red_size + this->green_size + this->blue_size;
  this->red   = malloc(n * sizeof(float));
  this->green = this->  red + this->  red_size;
  this->blue  = this->green + this->green_size;
  return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsf_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void libgamma_gamma_rampsf_destroy(libgamma_gamma_rampsf_t* restrict this)
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
void libgamma_gamma_rampsf_free(libgamma_gamma_rampsf_t* restrict this)
{
  free(this->red);
  free(this);
}



/**
 * Initialise a gamma ramp in the proper way that allows all adjustment
 * methods to read from and write to it without causing segmentation violation
 * 
 * The input must have `red_size`, `green_size` and `blue_size` set to the
 * sizes of the gamma ramps that should be allocated
 * 
 * @param   this  The gamma ramps
 * @return        Zero on success, -1 on allocation error, `errno` will be set accordingly
 */
int libgamma_gamma_rampsd_initialise(libgamma_gamma_rampsd_t* restrict this)
{
  size_t n = this->red_size + this->green_size + this->blue_size;
  this->red   = malloc(n * sizeof(double));
  this->green = this->  red + this->  red_size;
  this->blue  = this->green + this->green_size;
  return this->red ? 0 : -1;
}


/**
 * Release resources that are held by a gamma ramp strcuture that
 * has been allocated by `libgamma_gamma_rampsd_initialise` or otherwise
 * initialises in the proper manner
 * 
 * @param  this  The gamma ramps
 */
void libgamma_gamma_rampsd_destroy(libgamma_gamma_rampsd_t* restrict this)
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
void libgamma_gamma_rampsd_free(libgamma_gamma_rampsd_t* restrict this)
{
  free(this->red);
  free(this);
}

