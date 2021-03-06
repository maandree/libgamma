/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_VIDMODE
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
libgamma_x_vidmode_get_crtc_information(libgamma_crtc_information_t *restrict this,
                                        libgamma_crtc_state_t *restrict crtc, unsigned long long fields)
{
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)

	Display *restrict connection;
	int stops = 0;

	/* X VidMode does not support EDID or monitor dimensions */
	this->edid_error = _E(LIBGAMMA_CRTC_INFO_EDID);
	this->width_mm_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM);
	this->height_mm_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM);
	this->width_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID);
	this->height_mm_edid_error = _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID);
	this->gamma_size_error = 0;
	/* X VidMode does support gamma ramp size query. The gamma
	   ramps are identical but not fixed, and the query can fail. */
	if ((fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE)) {
		connection = crtc->partition->site->data;
		if (!XF86VidModeGetGammaRampSize(connection, (int)crtc->partition->partition, &stops))
			this->gamma_size_error = LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED;
		else if (stops < 2)
			this->gamma_size_error = LIBGAMMA_SINGLETON_GAMMA_RAMP;
		this->red_gamma_size = this->green_gamma_size = this->blue_gamma_size = (size_t)stops;
	}
	/* X VidMode uses 16-bit integer ramps */
	this->gamma_depth = 16;
	this->gamma_depth_error = 0;
	/* X VidMode does not support gamma ramp support queries */
	this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
	/* X VidMode does not support EDID or connector information */
	this->subpixel_order_error = _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER);
	this->active_error = _E(LIBGAMMA_CRTC_INFO_ACTIVE);
	this->connector_name_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);
	this->connector_type_error = _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE);
	this->gamma_error = _E(LIBGAMMA_CRTC_INFO_GAMMA);

	/* We failed if gamma ramp size query failed or if an unsupport field was queried. */
	return (this->gamma_size_error || (fields & ~(LIBGAMMA_CRTC_INFO_GAMMA_DEPTH | LIBGAMMA_CRTC_INFO_GAMMA_SIZE))) ? -1 : 0;

#undef _E
}
