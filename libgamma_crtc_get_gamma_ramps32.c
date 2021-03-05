/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_get_gamma_ramps32(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps32_t* restrict ramps)
{
#define DEPTH 32
#define TYPE bits32
#define APPEND_RAMPS(X) X##ramps32
#include "get_ramps.h"
}
