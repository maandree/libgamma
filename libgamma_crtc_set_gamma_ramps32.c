/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps32_t ramps)
{
#define DEPTH 32
#define RAMPS ramps32
#define TYPE bits32
#define APPEND_RAMPS(X) X##ramps32
#include "set_ramps.h"
}
