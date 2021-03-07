/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
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
libgamma_dummy_crtc_get_gamma_rampsd(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_rampsd *restrict ramps)
{
#define LIBGAMMA_DUMMY_GET_RAMPS
#define TYPE double
#define FIELD float_double
#define DEPTH -2
#include "method-dummy.h"
}
