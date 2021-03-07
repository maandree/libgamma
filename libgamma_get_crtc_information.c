/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   size    Should be `sizeof(*this)`, used to let the library know which version
 *                  of the structure is used so that it does not write outside of it
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
int
libgamma_get_crtc_information(struct libgamma_crtc_information *restrict this, size_t size,
                              struct libgamma_crtc_state *restrict crtc, unsigned long long fields)
{
	struct libgamma_crtc_information info_;
	int r, (*func)(struct libgamma_crtc_information *restrict, struct libgamma_crtc_state *restrict, unsigned long long);

	this->edid = NULL;
	this->connector_name = NULL;

	switch (crtc->partition->site->method) {
#define X(CONST, CNAME, ...)\
	case CONST:\
		func = libgamma_##CNAME##_get_crtc_information;\
		break;
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}

	if (size == sizeof(info_)) {
		r = func(this, crtc, fields);
		this->struct_version = LIBGAMMA_CRTC_INFORMATION_STRUCT_VERSION;
		return r;
	} else {
		info_.struct_version = LIBGAMMA_CRTC_INFORMATION_STRUCT_VERSION;
		r = func(&info_, crtc, fields);
		if (size < sizeof(info_)) {
			memcpy(this, &info_, size);
		} else {
			memcpy(this, &info_, sizeof(info_));
			memset(&((char *)this)[size], 0, size - sizeof(info_));
		}
		return r;
	}
}
