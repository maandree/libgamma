/* See LICENSE file for copyright and license details. */
#include "common.h"

#undef libgamma_crtc_set_gamma_ramps8
#undef libgamma_crtc_set_gamma_ramps16
#undef libgamma_crtc_set_gamma_ramps32
#undef libgamma_crtc_set_gamma_ramps64
#undef libgamma_crtc_set_gamma_rampsf
#undef libgamma_crtc_set_gamma_rampsd
#undef libgamma_get_crtc_information
#undef libgamma_method_capabilities


int libgamma_crtc_set_gamma_ramps8(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps8);
int libgamma_crtc_set_gamma_ramps16(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps16);
int libgamma_crtc_set_gamma_ramps32(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps32);
int libgamma_crtc_set_gamma_ramps64(struct libgamma_crtc_state *restrict, struct libgamma_gamma_ramps64);
int libgamma_crtc_set_gamma_rampsf(struct libgamma_crtc_state *restrict, struct libgamma_gamma_rampsf);
int libgamma_crtc_set_gamma_rampsd(struct libgamma_crtc_state *restrict, struct libgamma_gamma_rampsd);
int libgamma_get_crtc_information(struct libgamma_crtc_information *restrict, struct libgamma_crtc_state *restrict, int32_t);
int libgamma_method_capabilities(struct libgamma_method_capabilities__new *restrict, int);


int
libgamma_crtc_set_gamma_ramps8(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps8 ramps)
{
	return libgamma_crtc_set_gamma_ramps8__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_ramps16(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps16 ramps)
{
	return libgamma_crtc_set_gamma_ramps16__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_ramps32(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps32 ramps)
{
	return libgamma_crtc_set_gamma_ramps32__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_ramps64(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_ramps64 ramps)
{
	return libgamma_crtc_set_gamma_ramps64__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_rampsf(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_rampsf ramps)
{
	return libgamma_crtc_set_gamma_rampsf__new(this, &ramps);
}

int
libgamma_crtc_set_gamma_rampsd(struct libgamma_crtc_state *restrict this, struct libgamma_gamma_rampsd ramps)
{
	return libgamma_crtc_set_gamma_rampsd__new(this, &ramps);
}

int
libgamma_get_crtc_information(struct libgamma_crtc_information *restrict this,
                              struct libgamma_crtc_state *restrict crtc, int32_t fields)
{
	return libgamma_get_crtc_information__new(this, offsetof(struct libgamma_crtc_information, gamma_error) +
	                                          sizeof(this->gamma_error), crtc, (unsigned long long)fields);
}

int
libgamma_method_capabilities(struct libgamma_method_capabilities__new *restrict this, int method)
{
	const size_t size = (_Alignof(unsigned) > sizeof(int32_t) ? _Alignof(unsigned) : sizeof(int32_t)) + sizeof(unsigned);
	return libgamma_method_capabilities__new(this, size, method);
}
