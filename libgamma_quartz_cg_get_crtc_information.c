/* See LICENSE file for copyright and license details. */
#include "common.h"



/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read.
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`
 */
int
libgamma_quartz_cg_get_crtc_information(libgamma_crtc_information_t *restrict this,
                                        libgamma_crtc_state_t *restrict crtc, int32_t fields)
{
#define SUPPORTED_FIELDS (LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH)
#define _E(FIELD)        ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)

	CGDirectDisplayID *restrict crtcs;
	size_t gamma_size;

	/* Quartz/CoreGraphics does not support EDID or monitor dimensions */
	this->edid_error = _E(LIBGAMMA_CRTC_INFO_EDID);
	this->width_mm_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM);
	this->height_mm_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM);
	this->width_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID);
	this->height_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID);
	/* Quartz/CoreGraphics does support gamma ramp size query
	   The gamma ramps are identical but not fixed, and the query can fail */
	this->gamma_size_error = 0;
	if ((fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE)) {
		crtcs = crtc->partition->data;
		gamma_size = CGDisplayGammaTableCapacity(crtcs[crtc->crtc]);
		this->red_gamma_size = this->green_gamma_size = this->blue_gamma_size = (size_t)gamma_size;
		this->gamma_size_error = gamma_size < 2 ? LIBGAMMA_SINGLETON_GAMMA_RAMP : 0;
	}
	/* Quartz/CoreGraphics uses `float` ramps */
	this->gamma_depth = -1;
	this->gamma_depth_error = 0;
	/* Quartz/CoreGraphics does not support gamma ramp support queries */
	this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
	/* Quartz/CoreGraphics does not support EDID or connector information */
	this->subpixel_order_error = _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER);
	this->active_error = _E(LIBGAMMA_CRTC_INFO_ACTIVE);
	this->connector_name_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);
	this->connector_type_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE);
	this->gamma_error = _E(LIBGAMMA_CRTC_INFO_GAMMA);

	/* We failed if gamma ramp size query failed or if an unsupport field was queried */
	return (this->gamma_size_error || (fields & ~SUPPORTED_FIELDS)) ? -1 : 0;

#undef _E
#undef SUPPORTED_FIELDS
}
