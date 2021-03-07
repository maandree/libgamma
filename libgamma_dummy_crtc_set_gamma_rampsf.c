/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
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
libgamma_dummy_crtc_set_gamma_rampsf(struct libgamma_crtc_state *restrict this, const struct libgamma_gamma_rampsf *restrict ramps)
{
#define LIBGAMMA_DUMMY_SET_RAMPS
#define TYPE float
#define FIELD float_single
#define DEPTH -1
#include "method-dummy.h"
}
