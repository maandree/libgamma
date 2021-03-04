/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, 8-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_get_gamma_ramps8(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps8_t* restrict ramps)
{
#define DEPTH 8
#define RAMPS ramps8
#define TYPE bits8
#define APPEND_RAMPS(X) X##ramps8
#include "get_ramps.h"
}
