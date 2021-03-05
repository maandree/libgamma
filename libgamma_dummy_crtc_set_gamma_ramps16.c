/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_dummy_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict this, const libgamma_gamma_ramps16_t *restrict ramps)
{
#define LIBGAMMA_DUMMY_SET_RAMPS
#define TYPE uint16_t
#define FIELD bits16
#define DEPTH 16
#include "method-dummy.h"
}
