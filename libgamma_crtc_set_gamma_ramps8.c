/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, 8-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_set_gamma_ramps8(struct libgamma_crtc_state *restrict this, const struct libgamma_gamma_ramps8 *restrict ramps)
{
#define DEPTH 8
#define TYPE bits8
#define APPEND_RAMPS(X) X##ramps8
#include "set_ramps.h"
}
