/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_get_gamma_rampsd(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_rampsd *restrict ramps)
{
#define DEPTH -2
#define TYPE float_double
#define APPEND_RAMPS(X) X##rampsd
#include "get_ramps.h"
}
