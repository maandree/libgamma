/* See LICENSE file for copyright and license details. */
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
libgamma_dummy_get_crtc_information(libgamma_crtc_information_t *restrict this,
                                    libgamma_crtc_state_t *restrict crtc, int32_t fields)
{
	libgamma_dummy_crtc_t *restrict data = crtc->data;
	int supported = libgamma_dummy_internal_configurations.capabilities.crtc_information;
	int e = 0;
	size_t n;

	/* Copy over information */
	*this = data->info;

	/* Duplicate strings */
	if (this->edid) {
		this->edid = malloc(this->edid_length * sizeof(char));
		if (!this->edid)
			this->edid_error = errno;
		memcpy(this->edid, data->info.edid, this->edid_length * sizeof(char));
	}
	if (this->connector_name) {
		n = strlen(this->connector_name);
		this->connector_name = malloc((n + 1) * sizeof(char));
		if (!this->connector_name)
			this->connector_name_error = errno;
		memcpy(this->connector_name, data->info.connector_name, (n + 1) * sizeof(char));
	}

	/* Parse EDID */
	if (this->edid_error)
		this->width_mm_edid_error = this->height_mm_edid_error = this->gamma_error = this->edid_error;
	else if (fields & (LIBGAMMA_CRTC_INFO_MACRO_EDID ^ LIBGAMMA_CRTC_INFO_EDID))
		e |= libgamma_internal_parse_edid(this, fields);

	/* Test errors */
#define _E(FIELD, VAR)\
	((fields & FIELD) ? ((supported & FIELD) ? VAR : (VAR = LIBGAMMA_CRTC_INFO_NOT_SUPPORTED)) : 0)
	e |= _E(LIBGAMMA_CRTC_INFO_EDID,           this->edid_error);
	e |= _E(LIBGAMMA_CRTC_INFO_WIDTH_MM,       this->width_mm_error);
	e |= _E(LIBGAMMA_CRTC_INFO_HEIGHT_MM,      this->height_mm_error);
	e |= _E(LIBGAMMA_CRTC_INFO_GAMMA_SIZE,     this->gamma_size_error);
	e |= _E(LIBGAMMA_CRTC_INFO_GAMMA_DEPTH,    this->gamma_depth_error);
	e |= _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT,  this->gamma_support_error);
	e |= _E(LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER, this->subpixel_order_error);
	e |= _E(LIBGAMMA_CRTC_INFO_ACTIVE,         this->active_error);
	e |= _E(LIBGAMMA_CRTC_INFO_CONNECTOR_NAME, this->connector_name_error);
	e |= _E(LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE, this->connector_type_error);

	if ((fields & LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID) && !(supported & LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID))
		e |= this->width_mm_edid_error = LIBGAMMA_CRTC_INFO_NOT_SUPPORTED;
	if ((fields & LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID) && !(supported & LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID))
		e |= this->height_mm_edid_error = LIBGAMMA_CRTC_INFO_NOT_SUPPORTED;
	if ((fields & LIBGAMMA_CRTC_INFO_GAMMA) && !(supported & LIBGAMMA_CRTC_INFO_GAMMA))
		e |= this->gamma_error = LIBGAMMA_CRTC_INFO_NOT_SUPPORTED;
#undef _E

	return e ? -1 : 0;
}
