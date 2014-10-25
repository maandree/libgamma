/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014  Mattias Andrée (maandree@member.fsf.org)
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


/**
 * Just an arbitrary version.
 */
#define ANY  bits64

/**
 * Concatenation of all ramps.
 */
#define ALL red


/**
 * Preform installation in an `for (i = 0; i < n; i++)`
 * loop and do a `break` afterwords.
 */
#define __translate(instruction)  for (i = 0; i < n; i++)  instruction;  break


/**
 * Convert a [0, 1] `float` to a full range `uint64_t`
 * and mark sure rounding errors does not cause the
 * value be 0 instead of ~0 and vice versa.
 * 
 * @param   value  To `float` to convert.
 * @return         The value as an `uint64_t`.
 */
static inline uint64_t float_to_64(float value)
{
  /* XXX Which is faster? */
  
#if defined(HAVE_INT128) && __WORDSIZE == 64
  /* `__int128` is a GNU C extension, which
     (because it is not ISO C) emits a warning
     under -pedantic. */
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpedantic"
  
  /* In GCC we can use `__int128`, this is
     a signed 128-bit integer. It fits all
     uint64_t values but also native values,
     which is a nice because it eleminates
     some overflow condition tests. It is
     also more readable. */
  
  /* Convert to integer. */
  __int128 product = (__int128)(value * (float)UINT64_MAX);
  /* Negative overflow. */
  if (product > UINT64_MAX)
    return UINT64_MAX;
  /* Positive overflow. */
  if (product < 0)
    return 0;
  /* Did not overflow. */
  return (uint64_t)product;
  
# pragma GCC diagnostic pop
#else
  
  /* If we are not using GCC we cannot be
     sure that we have `__int128` so we have
     to use `uint64_t` and perform overflow
     checkes based on the input value. */
  
  /* Convert to integer. */
  uint64_t product = (uint64_t)(value * (float)UINT64_MAX);
  /* Negative overflow,
     if the input is less than 0,5 but
     the output is greater then we got
     -1 when we should have gotten 0. */
  if ((value < 0.1f) && (product > 0xF000000000000000ULL))
    return 0;
  /* Positive overflow,
     if the input is greater than 0,5
     but the output is less then we got
     0 when we should have gotten ~0. */
  else if ((value > 0.9f) && (product < 0x1000000000000000ULL))
    return (uint64_t)~0;
  /* Did not overflow. */
  return product;
  
#endif
}


/**
 * Convert a [0, 1] `double` to a full range `uint64_t`
 * and mark sure rounding errors does not cause the
 * value be 0 instead of ~0 and vice versa.
 * 
 * @param   value  To `double` to convert.
 * @return         The value as an `uint64_t`.
 */
static inline uint64_t double_to_64(double value)
{
  /* XXX Which is faster? */
  
#if defined(HAVE_INT128) && __WORDSIZE == 64
  /* `__int128` is a GNU C extension, which
     (because it is not ISO C) emits a warning
     under -pedantic. */
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wpedantic"
  
  /* In GCC we can use `__int128`, this is
     a signed 128-bit integer. It fits all
     uint64_t values but also native values,
     which is a nice because it eleminates
     some overflow condition tests. It is
     also more readable. */
  
  /* Convert to integer. */
  __int128 product = (__int128)(value * (double)UINT64_MAX);
  /* Negative overflow. */
  if (product > UINT64_MAX)
    return UINT64_MAX;
  /* Positive overflow. */
  if (product < 0)
    return 0;
  /* Did not overflow. */
  return (uint64_t)product;
  
# pragma GCC diagnostic pop
#else
  
  /* If we are not using GCC we cannot be
     sure that we have `__int128` so we have
     to use `uint64_t` and perform overflow
     checkes based on the input value. */
  
  /* Convert to integer. */
  uint64_t product = (uint64_t)(value * (double)UINT64_MAX);
  /* Negative overflow,
     if the input is less than 0,5 but
     the output is greater then we got
     -1 when we should have gotten 0. */
  if ((value < (double)0.1f) && (product > 0xF000000000000000ULL))
    product = 0;
  /* Positive overflow.
     if the input is greater than 0,5
     but the output is less then we got
     0 when we should have gotten ~0. */
  else if ((value > (double)0.9f) && (product < 0x1000000000000000ULL))
    product = (uint64_t)~0;
  /* Did not overflow. */
  return product;
  
#endif
}


/**
 * Convert any set of gamma ramps into a 64-bit integer array with all channels.
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`.
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes.)
 * @param  out    Output array.
 * @param  in     Input gamma ramps.
 */
static void translate_to_64(signed depth, size_t n, uint64_t* restrict out, libgamma_gamma_ramps_any_t in)
{
  size_t i;
  switch (depth)
    {
      /* Translate integer. */
    case  8:  __translate(out[i] = (uint64_t)(in.bits8. ALL[i]) * 0x0101010101010101ULL);
    case 16:  __translate(out[i] = (uint64_t)(in.bits16.ALL[i]) * 0x0001000100010001ULL);
    case 32:  __translate(out[i] = (uint64_t)(in.bits32.ALL[i]) * 0x0000000100000001ULL);
      /* Identity translation. */
    case 64:  __translate(out[i] = in.bits64.ALL[i]);
      /* Translate floating point. */
    case -1:  __translate(out[i] =  float_to_64(in.float_single.ALL[i]));
    case -2:  __translate(out[i] = double_to_64(in.float_double.ALL[i]));
    default:
      /* This is not possible. */
      abort();
      break;
    }
}


/**
 * Undo the actions of `translate_to_64`.
 * 
 * @param  depth  The depth of the gamma ramp, `-1` for `float`, `-2` for `double`.
 * @param  n      The grand size of gamma ramps (sum of all channels' sizes.)
 * @param  out    Output gamma ramps.
 * @param  in     Input array, may be modified.
 */
static void translate_from_64(signed depth, size_t n, libgamma_gamma_ramps_any_t out, uint64_t* restrict in)
{
  size_t i;
  switch (depth)
    {
      /* Translate integer. */
    case  8:  __translate(out.bits8. ALL[i] =  (uint8_t)(in[i] / 0x0101010101010101ULL));
    case 16:  __translate(out.bits16.ALL[i] = (uint16_t)(in[i] / 0x0001000100010001ULL));
    case 32:  __translate(out.bits32.ALL[i] = (uint32_t)(in[i] / 0x0000000100000001ULL));
      /* Identity translation. */
    case 64:  __translate(out.bits64.ALL[i] = in[i]);
      /* Translate floating point. */
    case -1:  __translate(out.float_single.ALL[i] =  (float)(in[i]) /  (float)UINT64_MAX);
    case -2:  __translate(out.float_double.ALL[i] = (double)(in[i]) / (double)UINT64_MAX);
    default:
      /* This is not possible. */
      abort();
      break;
    }
}


/**
 * Allocate and initalise a gamma ramp with any depth.
 * 
 * @param   ramps_sys  Output gamma ramps.
 * @param   ramps      The gamma ramps whose sizes should be duplicated.
 * @param   depth      The depth of the gamma ramps to allocate,
 *                     `-1` for `float`, `-2` for `double`.
 * @param   elements   Output reference for the grand size of the gamma ramps.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
static int allocated_any_ramp(libgamma_gamma_ramps_any_t* restrict ramps_sys,
			      libgamma_gamma_ramps_any_t ramps, signed depth, size_t* restrict elements)
{
  /* Calculate the size of the allocation to do. */
  size_t d, n = ramps.ANY.red_size + ramps.ANY.green_size + ramps.ANY.blue_size;
  switch (depth)
    {
    case  8:  d = sizeof(uint8_t);   break;
    case 16:  d = sizeof(uint16_t);  break;
    case 32:  d = sizeof(uint32_t);  break;
    case 64:  d = sizeof(uint64_t);  break;
    case -1:  d = sizeof(float);     break;
    case -2:  d = sizeof(double);    break;
    default:
      return errno = EINVAL, LIBGAMMA_ERRNO_SET;
    }
  
  /* Copy the gamma ramp sizes. */
  ramps_sys->ANY = ramps.ANY;
  /* Allocate the new ramps. */
#ifdef HAVE_LIBGAMMA_METHOD_LINUX_DRM
  /* Valgrind complains about us reading uninitialize memory if we just use malloc. */
  ramps_sys->ANY.red = calloc(n, d);
#else
  ramps_sys->ANY.red = malloc(n * d);
#endif
  ramps_sys->ANY.green = (void*)(((char*)(ramps_sys->ANY.  red)) + ramps.ANY.  red_size * d / sizeof(char));
  ramps_sys->ANY.blue  = (void*)(((char*)(ramps_sys->ANY.green)) + ramps.ANY.green_size * d / sizeof(char));
  
  /* Report the total gamma ramp size. */
  *elements = n;
  /* Report successfulness. */
  return ramps_sys->ANY.red == NULL ? LIBGAMMA_ERRNO_SET : 0;
}


/**
 * Get the current gamma ramps for a CRTC, re-encoding version.
 * 
 * @param   this          The CRTC state.
 * @param   ramps         The gamma ramps to fill with the current values.
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`.
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`.
 * @param   fun           Function that is to be used read the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical.
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library.
 */
int libgamma_translated_ramp_get_(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_any_t* restrict ramps,
				  signed depth_user, signed depth_system,
				  libgamma_get_ramps_any_fun* fun)
{
  size_t n;
  int r;
  libgamma_gamma_ramps_any_t ramps_sys;
  uint64_t* restrict ramps_full;
  
  /* Allocate ramps with proper data type. */
  if ((r = allocated_any_ramp(&ramps_sys, *ramps, depth_system, &n)))
    return r;
  
  /* Fill the ramps. */
  if ((r = fun(this, &ramps_sys)))
    return free(ramps_sys.ANY.red), r;
  
  /* Allocate intermediary ramps. */
  if ((ramps_full = malloc(n * sizeof(uint64_t))) == NULL)
    return free(ramps_sys.ANY.red), LIBGAMMA_ERRNO_SET;
  
  /* Translate ramps to 64-bit integers. */
  translate_to_64(depth_system, n, ramps_full, ramps_sys);
  free(ramps_sys.ANY.red);
  
  /* Translate ramps to the user's format. */
  translate_from_64(depth_user, n, *ramps, ramps_full);
  free(ramps_full);
  return 0;
}


/**
 * Set the gamma ramps for a CRTC, re-encoding version.
 * 
 * @param   this          The CRTC state.
 * @param   ramps         The gamma ramps to apply.
 * @param   depth_user    The depth of the gamma ramps that are provided by the user,
 *                        `-1` for `float`, `-2` for `double`.
 * @param   depth_system  The depth of the gamma ramps as required by the adjustment method,
 *                        `-1` for `float`, `-2` for `double`.
 * @param   fun           Function that is to be used write the ramps, its parameters have
 *                        the same function as those of this function with the same names,
 *                        and the return value too is identical.
 * @return                Zero on success, otherwise (negative) the value of an
 *                        error identifier provided by this library.
 */
int libgamma_translated_ramp_set_(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_any_t ramps,
				  signed depth_user, signed depth_system,
				  libgamma_set_ramps_any_fun* fun)
{
  size_t n;
  int r;
  libgamma_gamma_ramps_any_t ramps_sys;
  uint64_t* restrict ramps_full;
  
  /* Allocate ramps with proper data type. */
  if ((r = allocated_any_ramp(&ramps_sys, ramps, depth_system, &n)))
    return r;
  
  /* Allocate intermediary ramps. */
  if ((ramps_full = malloc(n * sizeof(uint64_t))) == NULL)
    return free(ramps_sys.ANY.red), LIBGAMMA_ERRNO_SET;
  
  /* Translate ramps to 64-bit integers. */
  translate_to_64(depth_user, n, ramps_full, ramps);
  /* Translate ramps to the proper format. */
  translate_from_64(depth_system, n, ramps_sys, ramps_full);
  free(ramps_full);
  
  /* Apply the ramps */
  r = fun(this, ramps_sys);
  
  free(ramps_sys.ANY.red);
  return r;
}


#undef __translate
#undef ALL
#undef ANY

