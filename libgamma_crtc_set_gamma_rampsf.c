/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_set_gamma_rampsf(struct libgamma_crtc_state *restrict this, const struct libgamma_gamma_rampsf *restrict ramps)
{
#define DEPTH -1
#define TYPE float_single
#define APPEND_RAMPS(X) X##rampsf
#include "set_ramps.h"
}
