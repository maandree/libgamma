/* See LICENSE file for copyright and license details. */
#include "common.h"



/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
int
libgamma_get_crtc_information(libgamma_crtc_information_t *restrict this, libgamma_crtc_state_t *restrict crtc, int32_t fields)
{
	this->edid = NULL;
	this->connector_name = NULL;

	switch (crtc->partition->site->method) {
#define X(CONST, CNAME, DEPTH, RAMPS)\
	case CONST:\
		return libgamma_##CNAME##_get_crtc_information(this, crtc, fields);
	LIST_AVAILABLE_METHODS(X)
#undef X
	default:
		return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
	}
}
