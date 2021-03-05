/* See LICENSE file for copyright and license details. */
#define IN_LIBGAMMA_X_RANDR
#include "common.h"


/**
 * Get the gamma ramp size of a CRTC
 * 
 * @param   this  Instance of a data structure to fill with the information about the CRTC
 * @param   crtc  The state of the CRTC whose information should be read
 * @return        Non-zero on error
 */
static int
get_gamma_ramp_size(libgamma_crtc_information_t *restrict out, libgamma_crtc_state_t *restrict crtc)
{
	xcb_connection_t *restrict connection = crtc->partition->site->data;
	xcb_randr_crtc_t *restrict crtc_id = crtc->data;
	xcb_randr_get_crtc_gamma_size_cookie_t cookie;
	xcb_randr_get_crtc_gamma_size_reply_t *restrict reply;
	xcb_generic_error_t *error;

	/* Query gamma ramp size */
	out->gamma_size_error = 0;
	cookie = xcb_randr_get_crtc_gamma_size(connection, *crtc_id);
	reply = xcb_randr_get_crtc_gamma_size_reply(connection, cookie, &error);
	if (error) {
		out->gamma_size_error = libgamma_x_randr_internal_translate_error(error->error_code,
		                                                                  LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED, 1);
		return out->gamma_size_error;
	}
	/* Sanity check gamma ramp size */
	if (reply->size < 2)
		out->gamma_size_error = LIBGAMMA_SINGLETON_GAMMA_RAMP;
	/* Store gamma ramp size */
	out->red_gamma_size = out->green_gamma_size = out->blue_gamma_size = reply->size;
	/* Release resources and return successfulnes */
	free(reply);
	return out->gamma_size_error;
}


/**
 * Read information from the CRTC's output
 * 
 * @param   out     Instance of a data structure to fill with the information about the CRTC
 * @param   output  The CRTC's output information
 * @return          Non-zero if at least on error occured
 */
static int
read_output_data(libgamma_crtc_information_t *restrict out, xcb_randr_get_output_info_reply_t *restrict output)
{
	switch (output->connection) {
	case XCB_RANDR_CONNECTION_CONNECTED:
		/* We have a monitor connected, report that and store information that is provided to us */
		out->active = 1;
		out->width_mm = output->mm_width;
		out->height_mm = output->mm_height;
		switch (output->subpixel_order) {
#define X(CONST, NAME, RENDER_SUFFIX)\
		case XCB_RENDER_SUB_PIXEL_##RENDER_SUFFIX:\
			out->subpixel_order = CONST;\
			break;
		LIST_SUBPIXEL_ORDERS(X)
#undef X
		default:
			out->subpixel_order_error = LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED;
			break;
		}
		return 0;

	case XCB_RANDR_CONNECTION_UNKNOWN:
		/* If we do know whether a monitor is connected report that and assume it is not */
		out->active_error = LIBGAMMA_STATE_UNKNOWN;
		/* fall through */
	default:
		/* If no monitor is connected, report that on fails that require it */
		out->width_mm_error       = LIBGAMMA_NOT_CONNECTED;
		out->height_mm_error      = LIBGAMMA_NOT_CONNECTED;
		out->subpixel_order_error = LIBGAMMA_NOT_CONNECTED;
		/* And store that we are not connected */
		out->active = 0;
		/* This fuction only failed if we could not figure out whether a monitor is connected */
		return output->connection != XCB_RANDR_CONNECTION_UNKNOWN ? 0 : -1;
	}
}


/**
 * Determine the connector type from the connector name
 * 
 * @param  this  The CRTC information to use and extend
 * @param        Non-zero on error
 */
static int
get_connector_type(libgamma_crtc_information_t *restrict this)
{
	/* Since we require the name of the output of get the type of the connected,
	   copy any reported error on the output's name to the connector's type,
	   and report failure if there was an error */
	if ((this->connector_type_error = this->connector_name_error))
		return -1;

#define __select(name, type)\
	do {\
		if (strstr(this->connector_name, name "-") == this->connector_name) {\
			this->connector_type = LIBGAMMA_CONNECTOR_TYPE_##type;\
			return 0;\
		}\
	} while (0)

	/* Check begin on the name of the output to find out what type the connector is of */
	__select ("None",        Unknown);
	__select ("VGA",         VGA);
	__select ("DVI-I",       DVII);
	__select ("DVI-D",       DVID);
	__select ("DVI-A",       DVIA);
	__select ("DVI",         DVI);
	__select ("Composite",   Composite);
	__select ("S-Video",     SVIDEO);
	__select ("Component",   Component);
	__select ("LFP",         LFP);
	__select ("Proprietary", Unknown);
	__select ("HDMI",        HDMI);
	__select ("DisplayPort", DisplayPort);

#undef __select

	/* If there was no matching output name pattern report that and exit with an error */
	this->connector_name_error = LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED;
	return -1;
}


/**
 * Get the output name of a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   output  The CRTC's output's information
 * @return          Non-zero on error
 */
static int
get_output_name(libgamma_crtc_information_t *restrict out, xcb_randr_get_output_info_reply_t *restrict output)
{
	char *restrict store;
	uint8_t *restrict name;
	uint16_t length;
	size_t i;

	/* Get the name of the output and the length of that name */
	name = xcb_randr_get_output_info_name(output);
	length = output->name_len; /* There is no NUL-termination */
	if (!name)
		return out->connector_name_error = LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED;

	/* Allocate a memory area for a NUL-terminated copy of the name */
	store = out->connector_name = malloc(((size_t)length + 1) * sizeof(char));
	if (!store) {
		out->connector_name_error = errno;
		return -1;
	}

	/* char is guaranteed to be (u)int_least8_t, but it is only guaranteed to be (u)int8_t
	 * on POSIX, so to be truly portable we will not assume that char is (u)int8_t */
	for (i = 0; i < (size_t)length; i++)
		store[i] = (char)name[i];
	store[length] = '\0';

	return 0;
}


/**
 * Get the Extended Display Information Data of the monitor connected to the connector of a CRTC
 * 
 * @param   out     Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   output  The CRTC's output
 * @return          Non-zero on error
 */
static int
get_edid(libgamma_crtc_information_t *restrict out, libgamma_crtc_state_t *restrict crtc, xcb_randr_output_t output)
{
	xcb_connection_t *restrict connection = crtc->partition->site->data;
	xcb_randr_list_output_properties_cookie_t prop_cookie;
	xcb_randr_list_output_properties_reply_t *restrict prop_reply;
	xcb_atom_t *atoms;
	xcb_atom_t *atoms_end;
	xcb_generic_error_t *error;
	xcb_get_atom_name_cookie_t atom_name_cookie;
	xcb_get_atom_name_reply_t *restrict atom_name_reply;
	char *restrict atom_name;
	int atom_name_len;
	xcb_randr_get_output_property_cookie_t atom_cookie;
	xcb_randr_get_output_property_reply_t *restrict atom_reply;
	unsigned char* restrict atom_data;
	int length;

	/* Acquire a list of all properties of the output */
	prop_cookie = xcb_randr_list_output_properties(connection, output);
	prop_reply = xcb_randr_list_output_properties_reply(connection, prop_cookie, &error);
	if (error)
		return out->edid_error = libgamma_x_randr_internal_translate_error(error->error_code, LIBGAMMA_LIST_PROPERTIES_FAILED, 1);
  
	/* Extract the properties form the data structure that holds them, */
	atoms = xcb_randr_list_output_properties_atoms(prop_reply);
	/* and get the last one so that we can iterate over them nicely */
	atoms_end = atoms + xcb_randr_list_output_properties_atoms_length(prop_reply);

	if (!atoms) {
		free(prop_reply);
		return out->edid_error = LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED;
	}

	/* For each property */
	for (; atoms != atoms_end; atoms++) {
		/* Acquire the atom name */
		atom_name_cookie = xcb_get_atom_name(connection, *atoms);
		atom_name_reply = xcb_get_atom_name_reply(connection, atom_name_cookie, &error);
		if (error)
			continue;

		/* Extract the atom name from the data structure that holds it */
		atom_name = xcb_get_atom_name_name(atom_name_reply);
		/* As well as the length of the name; it is not NUL-termianted */
		atom_name_len = xcb_get_atom_name_name_length(atom_name_reply);

		if (/* Check for errors */
		    !atom_name || /* atom_name_len < 1 || */
		    /* Check that the length is the expected length for the EDID property */
		    atom_name_len != 4 ||
		    /* Check that the property is the EDID property */
		    atom_name[0] != 'E' || atom_name[1] != 'D' || atom_name[2] != 'I' || atom_name[3] != 'D') {
			free(atom_name_reply);
			continue;
		}

		/* Acquire the property's value, we know that it is either 128 or 256 byte long */
		atom_cookie = xcb_randr_get_output_property(connection, output, *atoms, XCB_GET_PROPERTY_TYPE_ANY, 0, 256, 0, 0);
		atom_reply = xcb_randr_get_output_property_reply(connection, atom_cookie, &error);
		/* (*) EDID version 1.0 through 1.4 define it as 128 bytes long,
		 * but version 2.0 define it as 256 bytes long. However,
		 * version 2.0 is rare(?) and has been deprecated and replaced
		 * by version 1.3 (I guess that is with a new version epoch,
		 * but I do not know.) */
		if (error) {
			free(atom_name_reply);
			free(prop_reply);
			return out->edid_error = LIBGAMMA_PROPERTY_VALUE_QUERY_FAILED;
		}

		/* Extract the property's value */
		atom_data = xcb_randr_get_output_property_data(atom_reply);
		/* and its actual length */
		length = xcb_randr_get_output_property_data_length(atom_reply);
		if (!atom_data || length < 1) {
			free(atom_reply);
			free(atom_name_reply);
			free(prop_reply);
			return out->edid_error = LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED;
		}

		/* Store the EDID */
		out->edid_length = (size_t)length;
		out->edid = malloc((size_t)length * sizeof(unsigned char));
		if (!out->edid)
			out->edid_error = errno;
		else
			memcpy(out->edid, atom_data, (size_t)length * sizeof(unsigned char));

		/* Release resouces */
		free(atom_reply);
		free(atom_name_reply);
		free(prop_reply);

		return out->edid_error;
	}

	return out->edid_error = LIBGAMMA_EDID_NOT_FOUND;
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
libgamma_x_randr_get_crtc_information(libgamma_crtc_information_t *restrict this,
                                      libgamma_crtc_state_t *restrict crtc, int32_t fields)
{
#define _E(FIELD) ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)

	int e = 0;
	xcb_randr_get_output_info_reply_t *restrict output_info = NULL;
	xcb_randr_output_t output;
	int free_edid, free_name;
	xcb_connection_t *restrict connection;
	libgamma_x_randr_partition_data_t *restrict screen_data;
	size_t output_index;
	xcb_randr_get_output_info_cookie_t cookie;
	xcb_generic_error_t *error;

	/* Wipe all error indicators */
	memset(this, 0, sizeof(libgamma_crtc_information_t));

	/* We need to free the EDID after us if it is not explicitly requested */
	free_edid = !(fields & LIBGAMMA_CRTC_INFO_EDID);

	/* We need to free the output's name after us if it is not explicitly requested */
	free_name = !(fields & LIBGAMMA_CRTC_INFO_CONNECTOR_NAME);

	/* Jump if the output information is not required */
	if (!(fields & (LIBGAMMA_CRTC_INFO_MACRO_ACTIVE | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR)))
		goto cont;

	/* Get connector and connector information */
	connection = crtc->partition->site->data;
	screen_data = crtc->partition->data;
	output_index = screen_data->crtc_to_output[crtc->crtc];
	/* `SIZE_MAX` is used for CRTC:s that misses mapping to its output (should not happen),
	 * because `SIZE_MAX - 1` is the highest theoretical possible value */
	if (output_index == SIZE_MAX) {
		e |= this->edid_error = this->gamma_error = this->width_mm_edid_error
		   = this->height_mm_edid_error = this->connector_type_error
		   = this->connector_name_error = this->subpixel_order_error
		   = this->width_mm_error = this->height_mm_error
		   = this->active_error = LIBGAMMA_CONNECTOR_UNKNOWN;
		goto cont;
	}
	/* Get the output */
	output = screen_data->outputs[output_index];
	/* Query output information */
	cookie = xcb_randr_get_output_info(connection, output, screen_data->config_timestamp);
	output_info = xcb_randr_get_output_info_reply(connection, cookie, &error);
	if (error) {
		e |= this->edid_error = this->gamma_error = this->width_mm_edid_error
		   = this->height_mm_edid_error = this->connector_type_error
		   = this->connector_name_error = this->subpixel_order_error
		   = this->width_mm_error = this->height_mm_error
		   = this->active_error = LIBGAMMA_OUTPUT_INFORMATION_QUERY_FAILED;
		goto cont;
	}

	/* Get connector name */
	e |= get_output_name(this, output_info);
	/* Get connector type */
	if (fields & LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE)
		e |= get_connector_type(this);
	/* Get additional output data, excluding EDID */
	e |= read_output_data(this, output_info);
	if (fields & LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT)
		e |= this->width_mm_error | this->height_mm_error;
	e |= (fields & LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER) ? this->subpixel_order_error : 0;

	/* If we do not want any EDID information, jump */
	if (!(fields & LIBGAMMA_CRTC_INFO_MACRO_EDID))
		goto cont;
	/* If there is not monitor that report error in EDID related fields */
	if (!this->active) {
		e |= this->edid_error = this->gamma_error = this->width_mm_edid_error
		   = this->height_mm_edid_error = LIBGAMMA_NOT_CONNECTED;
		goto cont;
	}
	/* Get EDID */
	e |= get_edid(this, crtc, output);
	if (!this->edid) {
		this->gamma_error = this->width_mm_edid_error = this->height_mm_edid_error = this->edid_error;
		goto cont;
	}
	/* Parse EDID */
	if ((fields & (LIBGAMMA_CRTC_INFO_MACRO_EDID ^ LIBGAMMA_CRTC_INFO_EDID)))
		e |= libgamma_internal_parse_edid(this, fields);

cont:
	/* Get gamma ramp size */
	e |= (fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE) ? get_gamma_ramp_size(this, crtc) : 0;
	/* Store gamma ramp depth. */
	this->gamma_depth = 16;
	/* X RandR does not support quering gamma ramp support. */
	e |= this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);

	/* Free the EDID after us */
	if (free_edid) {
		free(this->edid);
		this->edid = NULL;
	}
	/* Free the output name after us */
	if (free_name) {
		free(this->connector_name);
		this->connector_name = NULL;
	}

	free(output_info);
	return e ? -1 : 0;

#undef _E
}
