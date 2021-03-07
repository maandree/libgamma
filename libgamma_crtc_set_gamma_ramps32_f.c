/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this            The CRTC state
 * @param   red_function    The function that generates the gamma ramp for the red channel
 * @param   green_function  The function that generates the gamma ramp for the green channel
 * @param   blue_function   The function that generates the gamma ramp for the blue channel
 * @return                  Zero on success, otherwise (negative) the value of an
 *                          error identifier provided by this library
 */
int
libgamma_crtc_set_gamma_ramps32_f(struct libgamma_crtc_state *restrict this, libgamma_gamma_ramps32_fun *red_function,
                                  libgamma_gamma_ramps32_fun *green_function, libgamma_gamma_ramps32_fun *blue_function)
{
#define TYPE uint32_t
#define APPEND_RAMPS(X) X##ramps32
#include "set_ramps_fun.h"
}
