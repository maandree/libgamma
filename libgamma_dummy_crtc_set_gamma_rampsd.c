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
libgamma_dummy_crtc_set_gamma_rampsd(libgamma_crtc_state_t *restrict this, const libgamma_gamma_rampsd_t *restrict ramps)
{
#define LIBGAMMA_DUMMY_SET_RAMPS
#define TYPE double
#define FIELD float_double
#define DEPTH -2
#include "method-dummy.h"
}
