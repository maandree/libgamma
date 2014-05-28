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
#ifndef LIBGAMMA_GAMMA_HELPER_H
#define LIBGAMMA_GAMMA_HELPER_H


#include "libgamma-method.h"


/**
 * Gamma ramp structure union for different depths.
 */
typedef union libgamma_gamma_ramps_any
{
  /**
   * 16-bit gamma ramps.
   */
  libgamma_gamma_ramps_t bits16;
  
  /**
   * 32-bit gamma ramps.
   */
  libgamma_gamma_ramps32_t bits32;
  
  /**
   * 64-bit gamma ramps.
   */
  libgamma_gamma_ramps64_t bits64;
  
  /**
   * Single precision float gamma ramps.
   */
  libgamma_gamma_rampsf_t float_single;
  
  /**
   * Double precision float gamma ramps.
   */
  libgamma_gamma_rampsd_t float_double;
  
} libgamma_gamma_ramps_any_t;


/**
 * A function for reading the gamma ramps from a CRTC.
 *
 * @param   this   The CRTC state.
 * @param   ramps  The store for the gamma ramps.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
typedef int libgamma_get_ramps_any_fun(libgamma_crtc_state_t* restrict this,
				       libgamma_gamma_ramps_any_t* restrict ramps);

/**
 * A function for writing the gamma ramps to a CRTC.
 *
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
typedef int libgamma_set_ramps_any_fun(libgamma_crtc_state_t* restrict this,
				       libgamma_gamma_ramps_any_t ramps);



/**
 * Get current the gamma ramps for a CRTC, re-encoding versio.n
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
#define libgamma_translated_ramp_get(this, ramps, depth_user, depth_system, fun)  \
  libgamma_translated_ramp_get_(this, ramps, depth_user, depth_system,            \
				(libgamma_get_ramps_any_fun*)(fun))


/**
 * Set the gamma ramps for a CRTC, re-encoding version
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
#define libgamma_translated_ramp_set(this, ramps, depth_user, depth_system, fun)  \
  libgamma_translated_ramp_set_(this, ramps, depth_user, depth_system,            \
				(libgamma_set_ramps_any_fun*)(fun))


/**
 * Get current the gamma ramps for a CRTC, re-encoding version.
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
				  libgamma_get_ramps_any_fun* fun);


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
				  libgamma_set_ramps_any_fun* fun);


#endif

