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
  size_t n = ramps->ANY.red_size + ramps->ANY.green_size + ramps->ANY.blue_size;
  size_t d, i;
  int r, e;
  libgamma_gamma_ramps_any_t ramps_sys;
  uint64_t* ramps_full;
  
  switch (depth_system)
    {
    case 16:  d = sizeof(uint16_t);  break;
    case 32:  d = sizeof(uint32_t);  break;
    case 64:  d = sizeof(uint64_t);  break;
    case -1:  d = sizeof(float);     break;
    case -2:  d = sizeof(double);    break;
    default:
      return errno = EINVAL, LIBGAMMA_ERRNO_SET;
    }
  
  ramps_sys.ANY.  red_size = ramps->ANY.  red_size;
  ramps_sys.ANY.green_size = ramps->ANY.green_size;
  ramps_sys.ANY. blue_size = ramps->ANY. blue_size;
  
  ramps_sys.ANY.red   = malloc(n * d);
  ramps_sys.ANY.green = (void*)(((char*)(ramps_sys.ANY.  red)) + ramps->ANY.  red_size * d / sizeof(char));
  ramps_sys.ANY.blue  = (void*)(((char*)(ramps_sys.ANY.green)) + ramps->ANY.green_size * d / sizeof(char));
  if (ramps_sys.ANY.red == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  if ((r = fun(this, &ramps_sys)))
    {
      e = errno;
      free(ramps_sys.ANY.red);
      return errno = e, r;
    }
  
  ramps_full = malloc(n * sizeof(uint64_t));
  if (ramps_full)
    {
      e = errno;
      free(ramps_sys.ANY.red);
      return errno = e, r;
    }
  
  switch (depth_system)
    {
    case 16:
      for (i = 0; i < n; i++)
	ramps_full[i] = (uint64_t)(ramps_sys.bits16.red[i]) * 0x0001000100010001ULL;
      break;
      
    case 32:
      for (i = 0; i < n; i++)
	ramps_full[i] = (uint64_t)(ramps_sys.bits32.red[i]) * 0x0000000100000001ULL;
      break;
      
    case 64:
      for (i = 0; i < n; i++)
	ramps_full[i] = ramps_sys.bits64.red[i];
      break;
      
    case -1:
      for (i = 0; i < n; i++)
	ramps_full[i] = (uint64_t)(ramps_sys.float_single.red[i] * (float)UINT64_MAX);
      break;
      
    case -2:
      for (i = 0; i < n; i++)
	ramps_full[i] = (uint64_t)(ramps_sys.float_double.red[i] * (double)UINT64_MAX);
      break;
      
    default: /* This is not possible. */
      abort();
      break;
    }
  
  free(ramps_sys.ANY.red);
  
  if ((0 < depth_user) && (depth_user < 64))
    for (i = 0; i < n; i++)
      {
	ramps_full[i] >>= (64 - depth_user - 1);
	ramps_full[i] = (ramps_full[i] & 1) | (ramps_full[i] >> 1);
      }
  
  switch (depth_user)
    {
    case 16:
      for (i = 0; i < n; i++)
	ramps->bits16.red[i] = (uint16_t)(ramps_full[i]);
      break;
      
    case 32:
      for (i = 0; i < n; i++)
	ramps->bits32.red[i] = (uint32_t)(ramps_full[i]);
      break;
      
    case 64:
      for (i = 0; i < n; i++)
	ramps->bits64.red[i] = ramps_full[i];
      break;
      
    case -1:
      for (i = 0; i < n; i++)
	ramps->float_single.red[i] = (float)(ramps_full[i]) / (float)UINT64_MAX;
      break;
      
    case -2:
      for (i = 0; i < n; i++)
	ramps->float_double.red[i] = (double)(ramps_full[i]) / (double)UINT64_MAX;
      break;
      
    default: /* This is not possible. */
      abort();
      break;
    }
  
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
}

#undef ANY

