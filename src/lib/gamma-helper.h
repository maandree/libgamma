/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_GAMMA_HELPER_H
#define LIBGAMMA_GAMMA_HELPER_H


#include "libgamma-method.h"


/**
 * Gamma ramp structure union for different depths
 */
typedef union libgamma_gamma_ramps_any {
	/**
	 * 8-bit gamma ramps
	 */
	libgamma_gamma_ramps8_t bits8;

	/**
	 * 16-bit gamma ramps
	 */
	libgamma_gamma_ramps16_t bits16;

	/**
	 * 32-bit gamma ramps
	 */
	libgamma_gamma_ramps32_t bits32;

	/**
	 * 64-bit gamma ramps
	 */
	libgamma_gamma_ramps64_t bits64;

	/**
	 * Single precision float gamma ramps
	 */
	libgamma_gamma_rampsf_t float_single;

	/**
	 * Double precision float gamma ramps
	 */
	libgamma_gamma_rampsd_t float_double;

} libgamma_gamma_ramps_any_t;


/**
 * A function for reading the gamma ramps from a CRTC
 *
 * @param   this   The CRTC state
 * @param   ramps  The store for the gamma ramps
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
typedef int libgamma_get_ramps_any_fun(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps_any_t *restrict);

/**
 * A function for writing the gamma ramps to a CRTC
 *
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
typedef int libgamma_set_ramps_any_fun(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps_any_t);



/**
 * Get the current gamma ramps for a CRTC, re-encoding version
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
#define libgamma_translated_ramp_get(this, ramps, depth_user, depth_system, fun)\
	libgamma_translated_ramp_get_((this), (ramps), (depth_user), (depth_system), (libgamma_get_ramps_any_fun *)(fun))


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
#define libgamma_translated_ramp_set(this, ramps, depth_user, depth_system, fun)\
	libgamma_translated_ramp_set_((this), (ramps), (depth_user), (depth_system), (libgamma_set_ramps_any_fun *)fun)


/**
 * Get the current gamma ramps for a CRTC, re-encoding version
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
int libgamma_translated_ramp_get_(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps_any_t *restrict,
                                  signed, signed, libgamma_get_ramps_any_fun *);


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
int libgamma_translated_ramp_set_(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps_any_t,
                                  signed, signed, libgamma_set_ramps_any_fun *);


#endif
