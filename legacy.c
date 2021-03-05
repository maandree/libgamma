/* See LICENSE file for copyright and license details. */
#include "common.h"

#undef libgamma_crtc_set_gamma_ramps8
#undef libgamma_crtc_set_gamma_ramps16
#undef libgamma_crtc_set_gamma_ramps32
#undef libgamma_crtc_set_gamma_ramps64
#undef libgamma_crtc_set_gamma_rampsf
#undef libgamma_crtc_set_gamma_rampsd


int libgamma_crtc_set_gamma_ramps8(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps8_t);
int libgamma_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps16_t);
int libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps32_t);
int libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t *restrict, libgamma_gamma_ramps64_t);
int libgamma_crtc_set_gamma_rampsf(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsf_t);
int libgamma_crtc_set_gamma_rampsd(libgamma_crtc_state_t *restrict, libgamma_gamma_rampsd_t);


int
libgamma_crtc_set_gamma_ramps8(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps8_t ramps)
{
	return libgamma_crtc_set_gamma_ramps8__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_ramps16(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps16_t ramps)
{
	return libgamma_crtc_set_gamma_ramps16__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps32_t ramps)
{
	return libgamma_crtc_set_gamma_ramps32__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t *restrict this, libgamma_gamma_ramps64_t ramps)
{
	return libgamma_crtc_set_gamma_ramps64__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_rampsf(libgamma_crtc_state_t *restrict this, libgamma_gamma_rampsf_t ramps)
{
	return libgamma_crtc_set_gamma_rampsf__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_rampsd(libgamma_crtc_state_t *restrict this, libgamma_gamma_rampsd_t ramps)
{
	return libgamma_crtc_set_gamma_rampsd__new(this, &ramps);
}
