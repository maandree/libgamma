/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_get_gamma_ramps64(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps64 *restrict ramps)
{
#define DEPTH 64
#define TYPE bits64
#define APPEND_RAMPS(X) X##ramps64
#include "get_ramps.h"
}
