/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_LINUX_DRM
#include "common.h"


/**
 * Find the connector that a CRTC belongs to
 * 
 * @param   this   The CRTC state
 * @param   error  Output of the error value to store of error report
 *                 fields for data that requires the connector
'* @return         The CRTC's conncetor, `NULL` on error
 */
static drmModeConnector *
find_connector(struct libgamma_crtc_state *restrict this, int *restrict error)
{
	uint32_t crtc_id = (uint32_t)(size_t)this->data;
	struct libgamma_drm_card_data *restrict card = this->partition->data;
	size_t i, n = (size_t)card->res->count_connectors;
	/* Open connectors and encoders if not already opened */
	if (!card->connectors) {
		/* Allocate connector and encoder arrays; we use `calloc`
		 * so all non-loaded elements are `NULL` after an error */
		card->connectors = calloc(n, sizeof(drmModeConnector *));
		if (!card->connectors)
			goto fail;
		card->encoders = calloc(n, sizeof(drmModeEncoder *));
		if (!card->encoders)
			goto fail;
		/* Fill connector and encoder arrays */
		for (i = 0; i < n; i++) {
			/* Get connector */
			card->connectors[i] = drmModeGetConnector(card->fd, card->res->connectors[i]);
			if (!card->connectors[i])
				goto fail;
			/* Get encoder if the connector is enabled. If it is disabled it
			 * will not have an encoder, which is indicated by the encoder
			 * ID being 0. In such case, leave the encoder to be `NULL`. */
			if (card->connectors[i]->encoder_id) {
				card->encoders[i] = drmModeGetEncoder(card->fd, card->connectors[i]->encoder_id);
				if (!card->encoders[i])
					goto fail;
			}
		}
	}
	/* No error has occurred yet */
	*error = 0;
	/* Find connector */
	for (i = 0; i < n; i++)
		if (card->encoders[i] && card->connectors[i] && card->encoders[i]->crtc_id == crtc_id)
			return card->connectors[i];
	/* We did not find the connector */
	*error = LIBGAMMA_CONNECTOR_UNKNOWN;
	return NULL;

fail:
	/* Report the error that got us here, release
	 * resouces and exit with `NULL` for failure */
	*error = errno;
	libgamma_linux_drm_internal_release_connectors_and_encoders(card);
	return NULL;
}


/**
 * Get the size of the gamma ramps for a CRTC
 * 
 * @param   out   Instance of a data structure to fill with the information about the CRTC
 * @param   crtc  The state of the CRTC whose information should be read
 * @return        The value stored in `out->gamma_size_error`
 */
static int
get_gamma_ramp_size(struct libgamma_crtc_information *restrict out, const struct libgamma_crtc_state *restrict crtc)
{
	struct libgamma_drm_card_data *restrict card = crtc->partition->data;
	uint32_t crtc_id = card->res->crtcs[crtc->crtc];
	drmModeCrtc *restrict crtc_info;
	/* Get CRTC information */
	errno = 0;
	crtc_info = drmModeGetCrtc(card->fd, crtc_id);
	out->gamma_size_error = crtc_info ? 0 : errno;
	/* Get gamma ramp size */
	if (!out->gamma_size_error) {
		/* Store gamma ramp size */
		out->red_gamma_size = out->green_gamma_size = out->blue_gamma_size = (size_t)crtc_info->gamma_size;
		/* Sanity check gamma ramp size */
		out->gamma_size_error = crtc_info->gamma_size < 2 ? LIBGAMMA_SINGLETON_GAMMA_RAMP : 0;
		/* Release CRTC information */
		drmModeFreeCrtc(crtc_info);
	}
	return out->gamma_size_error;
}


/**
 * Get the a monitor's subpixel order
 * 
 * @param  out        Instance of a data structure to fill with the information about the CRTC
 * @param  connector  The connector
 */
static void
get_subpixel_order(struct libgamma_crtc_information *restrict out, const drmModeConnector *restrict connector)
{
	switch (connector->subpixel) {
#define X(CONST, NAME, DRM_SUFFIX)\
	case DRM_MODE_SUBPIXEL_##DRM_SUFFIX:\
		out->subpixel_order = CONST;\
		break;
	LIST_SUBPIXEL_ORDERS(X)
	default:
		out->subpixel_order_error = LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED;
		break;
#undef X
	}
}


/**
 * Get a connector's type
 * 
 * @param  out                  Instance of a data structure to fill with the information about the CRTC
 * @param  connector            The connector
 * @param  connector_name_base  Output for the basename of the connector
 */
static void
get_connector_type(struct libgamma_crtc_information *restrict out, const drmModeConnector *restrict connector,
                   const char **restrict connector_name_base)
{
	/* These may not have been included by <xf86drmMode.h>,
	   but they should be available. Because we define them
	   ourself, it is best to test them last. */
#ifndef DRM_MODE_CONNECTOR_VIRTUAL
# define DRM_MODE_CONNECTOR_VIRTUAL 15
#endif
#ifndef DRM_MODE_CONNECTOR_DSI
# define DRM_MODE_CONNECTOR_DSI 16
#endif
#ifndef DRM_MODE_CONNECTOR_DPI
# define DRM_MODE_CONNECTOR_DPI 17
#endif
#ifndef DRM_MODE_CONNECTOR_WRITEBACK
# define DRM_MODE_CONNECTOR_WRITEBACK 18
#endif
#ifndef DRM_MODE_CONNECTOR_SPI
# define DRM_MODE_CONNECTOR_SPI 19
#endif

	/* These are not defined */
#ifndef DRM_MODE_CONNECTOR_DVI
# define DRM_MODE_CONNECTOR_DVI 123000001
#elif defined(__GNUC__)
# warning DRM_MODE_CONNECTOR_DVI is defined, update fallback definitions
#endif
#ifndef DRM_MODE_CONNECTOR_HDMI
# define DRM_MODE_CONNECTOR_HDMI 123000002
#elif defined(__GNUC__)
# warning DRM_MODE_CONNECTOR_HDMI is defined, update fallback definitions
#endif
#ifndef DRM_MODE_CONNECTOR_LFP
# define DRM_MODE_CONNECTOR_LFP 123000003
#elif defined(__GNUC__)
# warning DRM_MODE_CONNECTOR_LFP is defined, update fallback definitions
#endif

	switch (connector->connector_type) {
#define X(CONST, NAME, DRM_SUFFIX)\
	case DRM_MODE_CONNECTOR_##DRM_SUFFIX:\
		out->connector_type = CONST;\
		*connector_name_base = NAME;\
		break;
	LIST_CONNECTOR_TYPES(X)
#undef X
	default:
		out->connector_type_error = LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED;
		out->connector_name_error = LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED;
		break;
	}
}


/**
 * Read information from the CRTC's conncetor
 * 
 * @param   crtc       The state of the CRTC whose information should be read
 * @param   out        Instance of a data structure to fill with the information about the CRTC
 * @param   connector  The CRTC's connector
 * @param   fields     OR:ed identifiers for the information about the CRTC that should be read
 * @return             Non-zero if at least on error occured
 */
static int
read_connector_data(struct libgamma_crtc_state *restrict crtc, struct libgamma_crtc_information *restrict out,
                    const drmModeConnector *restrict connector, unsigned long long int fields)
{
	const char *connector_name_base = NULL;
	struct libgamma_drm_card_data *restrict card;
	uint32_t type;
	size_t i, n, c, len;

	/* Get some information that does not require too much work */
	if (fields & (LIBGAMMA_CRTC_INFO_MACRO_ACTIVE | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR)) {
		/* Get whether or not a monitor is plugged in */
		out->active = connector->connection == DRM_MODE_CONNECTED;
		out->active_error = connector->connection == DRM_MODE_UNKNOWNCONNECTION ? LIBGAMMA_STATE_UNKNOWN : 0;
		if (!out->active) {
			if (fields & (LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT | LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER))
				out->width_mm_error = out->height_mm_error = out->subpixel_order_error = LIBGAMMA_NOT_CONNECTED;
			goto not_connected;
		}

		/* Get viewport dimension */
		out->width_mm = connector->mmWidth;
		out->height_mm = connector->mmHeight;

		/* Get subpixel order */
		get_subpixel_order(out, connector);

	not_connected:

		/* Get connector type */
		get_connector_type(out, connector, &connector_name_base);
	}

	/* Get the connector's name */
	if ((fields & LIBGAMMA_CRTC_INFO_CONNECTOR_NAME) && !out->connector_name_error) {
		card = crtc->partition->data;
		type = connector->connector_type;
		n = (size_t)card->res->count_connectors;

		/* Allocate memory for the name of the connector */
		len = strlen(connector_name_base);
		if (len > SIZE_MAX / sizeof(char) - 12) {
			errno = ENOMEM;
			out->connector_name = NULL;
			return (out->connector_name_error = errno);
		} else {
			out->connector_name = malloc((len + 12) * sizeof(char));
			if (!out->connector_name)
				return (out->connector_name_error = errno);
		}

		/* Get the number of connectors with the same type on the same graphics card */
		for (i = c = 0; i < n && card->connectors[i] != connector; i++)
			if (card->connectors[i]->connector_type == type)
				c++;

		/* Construct and store connect name that is unique to the graphics card */
		sprintf(out->connector_name, "%s-%" PRIu32, connector_name_base, (uint32_t)(c + 1));
	}

	/* Did something go wrong? */
	return out->subpixel_order_error | out->active_error | out->connector_name_error;
}


/**
 * Get the extended display identification data for a monitor
 * 
 * @param   crtc       The CRTC state
 * @param   out        Instance of a data structure to fill with the information about the CRTC
 * @param   connector  The CRTC's connector
 * @reutnr             Non-zero on error
 */
static int
get_edid(struct libgamma_crtc_state *restrict crtc, struct libgamma_crtc_information *restrict out, drmModeConnector *connector)
{
	struct libgamma_drm_card_data *restrict card = crtc->partition->data;
	int prop_n = connector->count_props;
	int prop_i;
	drmModePropertyRes *restrict prop;
	drmModePropertyBlobRes *restrict blob;

	/* Test all properies on the connector */
	for (prop_i = 0; prop_i < prop_n; prop_i++) {
		/* Get output property */
		prop = drmModeGetProperty(card->fd, connector->props[prop_i]);
		if (!prop)
			continue;
		/* Is this property the EDID? */
		if (!strcmp(prop->name, "EDID")) {
			/* Get the property value */
			blob = drmModeGetPropertyBlob(card->fd, (uint32_t)connector->prop_values[prop_i]);
			if (!blob) {
				drmModeFreeProperty(prop);
				return (out->edid_error = LIBGAMMA_PROPERTY_VALUE_QUERY_FAILED);
			}
			if (blob->data) {
				/* Get and store the length of the EDID */
				out->edid_length = blob->length;
				/* Allocate memory for a copy of the EDID that is under our memory control */
				out->edid = malloc(out->edid_length * sizeof(unsigned char));
				if (!out->edid) {
					out->edid_error = errno;
				} else {
					/* Copy the EDID so we can free resources that got us here */
					memcpy(out->edid, blob->data, (size_t)out->edid_length * sizeof(unsigned char));
				}
				/* Free the propriety value and the propery */
				drmModeFreePropertyBlob(blob);
				drmModeFreeProperty(prop);
				/* Were we successful? */
				return !out->edid;
			}
			/* Free the propriety value */
			drmModeFreePropertyBlob(blob);
		}
		/* Free the propriety */
		drmModeFreeProperty(prop);
	}
	/* If we get here, we did not find a EDID */
	return (out->edid_error = LIBGAMMA_EDID_NOT_FOUND);
}


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error; on error refer to the error reports in `this`
 */
int
libgamma_linux_drm_get_crtc_information(struct libgamma_crtc_information *restrict this,
                                        struct libgamma_crtc_state *restrict crtc, unsigned long long fields)
{
#define _E(FIELD) ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)

	int e = 0, require_connector, free_edid, error;
	drmModeConnector *restrict connector;

	/* Wipe all error indicators */
	memset(this, 0, sizeof(*this));

	/* We need to free the EDID after us if it is not explicitly requested */
	free_edid = !(fields & LIBGAMMA_CRTC_INFO_EDID);

	/* Figure out whether we require the connector to get all information we want */
	require_connector = fields & (LIBGAMMA_CRTC_INFO_MACRO_ACTIVE | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR);

	/* If we are not interested in the connector or monitor, jump */
	if (!require_connector)
		goto cont;
	/* Find connector. */
	connector = find_connector(crtc, &error);
	if (!connector) {
		/* Store reported error in affected fields */
		e |= this->width_mm_error       = this->height_mm_error
		   = this->connector_type_error = this->subpixel_order_error
		   = this->active_error         = this->connector_name_error
		   = this->edid_error           = this->gamma_error
		   = this->width_mm_edid_error  = this->height_mm_edid_error = error;
		goto cont;
	}

	/* Read connector data and monitor data, excluding EDID */
	e |= read_connector_data(crtc, this, connector, fields);

	/* If we do not want any EDID information, jump */
	if (!(fields & LIBGAMMA_CRTC_INFO_MACRO_EDID))
		goto cont;
	/* If there is not monitor that report error in EDID related fields */
	if (this->active_error || !this->active) {
		e |= this->edid_error = this->gamma_error
		   = this->width_mm_edid_error = this->height_mm_edid_error
		   = LIBGAMMA_NOT_CONNECTED;
		goto cont;
	}
	/* Get EDID */
	e |= get_edid(crtc, this, connector);
	if (!this->edid) {
		this->gamma_error = this->width_mm_edid_error = this->height_mm_edid_error = this->edid_error;
		goto cont;
	}
	/* Parse EDID */
	if (fields & (LIBGAMMA_CRTC_INFO_MACRO_EDID ^ LIBGAMMA_CRTC_INFO_EDID))
		e |= libgamma_internal_parse_edid(this, fields);

cont:
	/* Get gamma ramp size */
	e |= (fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE) ? get_gamma_ramp_size(this, crtc) : 0;
	/* Store gamma ramp depth */
	this->gamma_depth = 16;
	/* DRM does not support quering gamma ramp support */
	e |= this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);

	/* Free the EDID after us */
	if (free_edid) {
		free(this->edid);
		this->edid = NULL;
	}

	return e ? -1 : 0;

#undef _E
}
