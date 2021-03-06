/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Return the capabilities of an adjustment method
 * 
 * @param   this    The data structure to fill with the method's capabilities
 * @param   size    Should be `sizeof(*this)`, used to let the library know which version
 *                  of the structure is used so that it does not write outside of it
 * @param   method  The adjustment method (display server and protocol)
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int
libgamma_method_capabilities(libgamma_method_capabilities_t *restrict this, size_t size, int method)
{
	libgamma_method_capabilities_t caps_;
	void (*func)(libgamma_method_capabilities_t *restrict);

	memset(this, 0, sizeof(*this));

	switch (method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		func = libgamma_##CNAME##_method_capabilities;\
		break;
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}

	if (size == sizeof(caps_)) {
		func(this);
		this->struct_version = LIBGAMMA_METHOD_CAPABILITIES_STRUCT_VERSION;
	} else {
		func(&caps_);
		caps_.struct_version = LIBGAMMA_METHOD_CAPABILITIES_STRUCT_VERSION;
		if (size < sizeof(caps_)) {
			memcpy(this, &caps_, size);
		} else {
			memcpy(this, &caps_, sizeof(caps_));
			memset(&((char *)this)[size], 0, size - sizeof(caps_));
		}
	}
	this->crtc_information__old = (int32_t)this->crtc_information;
}
