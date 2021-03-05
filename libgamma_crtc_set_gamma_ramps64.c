/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t *restrict this, const libgamma_gamma_ramps64_t *restrict ramps)
{
#define DEPTH 64
#define TYPE bits64
#define APPEND_RAMPS(X) X##ramps64
#include "set_ramps.h"
}
