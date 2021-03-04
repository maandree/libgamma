/* See LICENSE file for copyright and license details. */

/*
 * This file is intended to be included from
 * libgamma_crtc_set_gamma_ramps{8,16,32,64,f,d}
 */


gamma_ramps_any_t ramps_;
switch (this->partition->site->method) {
#define X(CONST, CNAME, MDEPTH, MRAMPS)\
case CONST:\
	if (!(MDEPTH)) {\
		return APPEND_RAMPS(libgamma_dummy_crtc_set_gamma_)(this, ramps);\
	} else if ((DEPTH) == (MDEPTH)) {\
		return libgamma_##CNAME##_crtc_set_gamma_##MRAMPS(this, *(libgamma_gamma_##MRAMPS##_t *)&ramps); \
	} else {\
		ramps_.TYPE = ramps;\
		return libgamma_internal_translated_ramp_set(this, ramps_, DEPTH, MDEPTH, libgamma_crtc_set_gamma_##MRAMPS);\
	}
LIST_AVAILABLE_METHODS(X)
#undef X
default:
	return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
}
