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
#include "gamma-helper.h"

#include "libgamma-method.h"
#include "libgamma-error.h"

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>


#define ANY  bits64


/**
 * Convert any set of gamma ramps into a 64-bit integer array with all channels
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes)
 * @param  out    Output array
 * @param  in     Input gamma ramps
 */
static void translate_to_64(signed depth, size_t n, uint64_t* restrict out, libgamma_gamma_ramps_any_t in)
{
  size_t i;
  switch (depth)
    {
    case 16:
      for (i = 0; i < n; i++)
	out[i] = (uint64_t)(in.bits16.red[i]) * 0x0001000100010001ULL;
      break;
      
    case 32:
      for (i = 0; i < n; i++)
	out[i] = (uint64_t)(in.bits32.red[i]) * 0x0000000100000001ULL;
      break;
      
    case 64:
      for (i = 0; i < n; i++)
	out[i] = in.bits64.red[i];
      break;
      
    case -1:
      for (i = 0; i < n; i++)
	out[i] = (uint64_t)(in.float_single.red[i] * (float)UINT64_MAX);
      break;
      
    case -2:
      for (i = 0; i < n; i++)
	out[i] = (uint64_t)(in.float_double.red[i] * (double)UINT64_MAX);
      break;
      
    default: /* This is not possible. */
      abort();
      break;
    }
}


/**
 * Undo the actions of `translate_to_64`
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes)
 * @param  out    Output gamma ramps
 * @param  in     Input array, may be modified
 */
static void translate_from_64(signed depth, size_t n, libgamma_gamma_ramps_any_t out, uint64_t* restrict in)
{
  size_t i;
  
  switch (depth)
    {
    case 16:
      for (i = 0; i < n; i++)
	in[i] >>= (64 - depth - 1),
	out.bits16.red[i] = (uint16_t)((in[i] & 1) | (in[i] >> 1));
      break;
      
    case 32:
      for (i = 0; i < n; i++)
	in[i] >>= (64 - depth - 1),
	out.bits32.red[i] = (uint32_t)((in[i] & 1) | (in[i] >> 1));
      break;
      
    case 64:
      for (i = 0; i < n; i++)
	out.bits64.red[i] = in[i];
      break;
      
    case -1:
      for (i = 0; i < n; i++)
	out.float_single.red[i] = (float)(in[i]) / (float)UINT64_MAX;
      break;
      
    case -2:
      for (i = 0; i < n; i++)
	out.float_double.red[i] = (double)(in[i]) / (double)UINT64_MAX;
      break;
      
    default: /* This is not possible. */
      abort();
      break;
    }
}


/**
 * Allocate and initalise a gamma ramp with any depth
 * 
 * @param   ramps_sys  Output gamma ramps
 * @param   ramps      The gamma ramps whose sizes should be duplicated
 * @param   depth      The depth of the gamma ramps to allocate,
 *                     `-1` for `float`, `-2` for `double`
 * @param   elements   Output reference for the grand size of the gamma ramps
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
static int allocated_any_ramp(libgamma_gamma_ramps_any_t* restrict ramps_sys,
			      libgamma_gamma_ramps_any_t ramps, signed depth, size_t* restrict elements)
{
  size_t n = 0;
  size_t d;
  
  switch (depth)
    {
    case 16:  d = sizeof(uint16_t);  break;
    case 32:  d = sizeof(uint32_t);  break;
    case 64:  d = sizeof(uint64_t);  break;
    case -1:  d = sizeof(float);     break;
    case -2:  d = sizeof(double);    break;
    default:
      return errno = EINVAL, LIBGAMMA_ERRNO_SET;
    }
  
  n += ramps_sys->ANY.  red_size = ramps.ANY.  red_size;
  n += ramps_sys->ANY.green_size = ramps.ANY.green_size;
  n += ramps_sys->ANY. blue_size = ramps.ANY. blue_size;
  
  ramps_sys->ANY.red   = malloc(n * d);
  ramps_sys->ANY.green = (void*)(((char*)(ramps_sys->ANY.  red)) + ramps.ANY.  red_size * d / sizeof(char));
  ramps_sys->ANY.blue  = (void*)(((char*)(ramps_sys->ANY.green)) + ramps.ANY.green_size * d / sizeof(char));
  
  *elements = n;
  return ramps_sys->ANY.red ? 0 : LIBGAMMA_ERRNO_SET;
}


/**
 * Get current the gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to fill with the current values
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used read the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
int libgamma_translated_ramp_get_(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_any_t* restrict ramps,
				  signed depth_user, signed depth_system,
				  libgamma_get_ramps_any_fun* fun)
{
  size_t n;
  int r;
  libgamma_gamma_ramps_any_t ramps_sys;
  uint64_t* ramps_full;
  
  if ((r = allocated_any_ramp(&ramps_sys, *ramps, depth_system, &n)))
    return r;
  
  if ((r = fun(this, &ramps_sys)))
    {
      free(ramps_sys.ANY.red);
      return r;
    }
  
  if ((ramps_full = malloc(n * sizeof(uint64_t))) == NULL)
    {
      free(ramps_sys.ANY.red);
      return LIBGAMMA_ERRNO_SET;
    }
  
  translate_to_64(depth_system, n, ramps_full, ramps_sys);
  free(ramps_sys.ANY.red);
  
  translate_from_64(depth_user, n, *ramps, ramps_full);
  free(ramps_full);
  return 0;
}


/**
 * Set the gamma ramps for a CRTC, re-encoding version
 * 
 * @param   this          The CRTC state
 * @param   ramps         The gamma ramps to apply
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`
 * @param   fun           Function that is to be used write the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library
 */
int libgamma_translated_ramp_set_(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_any_t ramps,
				  signed depth_user, signed depth_system,
				  libgamma_set_ramps_any_fun* fun)
{
  size_t n;
  int r;
  libgamma_gamma_ramps_any_t ramps_sys;
  uint64_t* ramps_full;
  
  if ((r = allocated_any_ramp(&ramps_sys, ramps, depth_system, &n)))
    return r;
  
  if ((ramps_full = malloc(n * sizeof(uint64_t))) == NULL)
    {
      free(ramps_sys.ANY.red);
      return LIBGAMMA_ERRNO_SET;
    }
  
  translate_to_64(depth_user, n, ramps_full, ramps);
  translate_from_64(depth_system, n, ramps_sys, ramps_full);
  free(ramps_full);
  
  r = fun(this, ramps_sys);
  
  free(ramps_sys.ANY.red);
  return r;
}


#undef ANY

