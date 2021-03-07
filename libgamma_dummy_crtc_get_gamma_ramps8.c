/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
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
libgamma_dummy_crtc_get_gamma_ramps8(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps8 *restrict ramps)
{
#define LIBGAMMA_DUMMY_GET_RAMPS
#define TYPE uint8_t
#define FIELD bits8
#define DEPTH 8
#include "method-dummy.h"
}
