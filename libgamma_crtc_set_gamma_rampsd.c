/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_crtc_set_gamma_rampsd(libgamma_crtc_state_t *restrict this, libgamma_gamma_rampsd_t ramps)
{
#define DEPTH -2
#define RAMPS rampsd
#define TYPE float_double
#define APPEND_RAMPS(X) X##rampsd
#include "set_ramps.h"
}
