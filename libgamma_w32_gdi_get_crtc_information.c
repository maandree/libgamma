/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_W32_GDI
#include "common.h"



/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`
 */
int
libgamma_w32_gdi_get_crtc_information(libgamma_crtc_information_t *restrict this,
                                      libgamma_crtc_state_t *restrict crtc, int32_t fields)
{
#define KNOWN_FIELDS (LIBGAMMA_CRTC_INFO_GAMMA_SIZE | LIBGAMMA_CRTC_INFO_GAMMA_DEPTH)
#define _E(FIELD)    ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)

	(void) crtc;

	/* Windows GDI does not support EDID or monitor dimensions */
	this->edid_error = _E(LIBGAMMA_CRTC_INFO_EDID);
	this->width_mm_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM);
	this->height_mm_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM);
	this->width_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID);
	this->height_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID);
	/* Windows GDI have fixed gamma ramp sizes */
	this->red_gamma_size = W32_GDI_GAMMA_RAMP_SIZE;
	this->green_gamma_size = W32_GDI_GAMMA_RAMP_SIZE;
	this->blue_gamma_size = W32_GDI_GAMMA_RAMP_SIZE;
	this->gamma_size_error = 0;
	/* Windows GDI have fixed gamma ramp depth */
	this->gamma_depth = 16;
	this->gamma_depth_error = 0;
	/* It is possible to query Windows GDI whether the device
	   have gamma ramp support. It cannot fail. However, I think
	   the result is incorrect if multiple monitors are active,
	   so we cannot include this. */
	/*
	if ((fields & LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT))
		this->gamma_support = GetDeviceCaps(crtc->data, COLORMGMTCAPS) == CM_GAMMA_RAMP;
	this->gamma_support_error = 0;
	*/
	this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
	/* Windows GDI does not support EDID or connector information */
	this->subpixel_order_error = _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER);
	this->active_error = _E(LIBGAMMA_CRTC_INFO_ACTIVE);
	this->connector_name_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);
	this->connector_type_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE);
	this->gamma_error = _E(LIBGAMMA_CRTC_INFO_GAMMA);

	/* There was a failure if and only if unsupport field was requested. */
	return (fields & ~KNOWN_FIELDS) ? -1 : 0;

#undef _E
#undef KNOWN_FIELDS
}
