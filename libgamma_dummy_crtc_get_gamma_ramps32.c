/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_DUMMY
#include "common.h"


/**
 * Get the current gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int
libgamma_dummy_crtc_get_gamma_ramps32(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps32_t *restrict ramps)
{
#define LIBGAMMA_DUMMY_GET_RAMPS
#define TYPE uint32_t
#define FIELD bits32
#define DEPTH 32
#include "method-dummy.h"
}
