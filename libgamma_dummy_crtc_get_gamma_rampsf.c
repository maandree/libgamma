/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_dummy_crtc_get_gamma_rampsf(libgamma_crtc_state_t *restrict this, libgamma_gamma_rampsf_t *restrict ramps)
{
#define LIBGAMMA_DUMMY_GET_RAMPS
#define TYPE float
#define FIELD float_single
#define DEPTH -1
#include "method-dummy.h"
}
