/**
 * libgamma — Display server abstraction layer for gamma ramp adjustments
 * Copyright © 2014  Mattias Andrée (maandree@member.fsf.org)
 * 
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef HAVE_GAMMA_METHOD_X_RANDR
# error Compiling gamma-x-randr.c without HAVE_GAMMA_METHOD_X_RANDR
#endif

#include "gamma-x-randr.h"

#include "libgamma-error.h"
#include "edid.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#ifdef DEBUG
# include <stdio.h>
#endif

#include <xcb/xcb.h>
#include <xcb/randr.h>


/**
 * The major version of RandR the library expects
 */
#define RANDR_VERSION_MAJOR  1

/**
 * The minor version of RandR the library expects
 */
#define RANDR_VERSION_MINOR  3



/**
 * Translate an xcb error into a libgamma error
 * 
 * @param   error_code     The xcb error
 * @param   default_error  The libgamma error to use if the xcb error is not recognised
 * @return                 The libgamma error
 */
static int translate_error_(int error_code, int default_error)
{
  switch (error_code)
    {
    case XCB_CONN_ERROR:                    return errno = ECONNABORTED, LIBGAMMA_ERRNO_SET;
    case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:  return errno = ENOPROTOOPT, LIBGAMMA_ERRNO_SET;
    case XCB_CONN_CLOSED_MEM_INSUFFICIENT:  return errno = ENOMEM, LIBGAMMA_ERRNO_SET;
    case XCB_CONN_CLOSED_REQ_LEN_EXCEED:    return errno = EMSGSIZE, LIBGAMMA_ERRNO_SET;
    case XCB_CONN_CLOSED_PARSE_ERR:         return LIBGAMMA_NO_SUCH_SITE;
    case XCB_CONN_CLOSED_INVALID_SCREEN:    return LIBGAMMA_NO_SUCH_PARTITION;
    case XCB_CONN_CLOSED_FDPASSING_FAILED:  return errno = EIO, LIBGAMMA_ERRNO_SET;
    default:
      return default_error;
    }
}


/**
 * Translate an xcb error into a libgamma error
 * 
 * @param   error_code     The xcb error
 * @param   default_error  The libgamma error to use if the xcb error is not recognised
 * @param   return_errno   Whether an `errno` value may be returned
 * @return                 The libgamma error
 */
static int translate_error(int error_code, int default_error, int return_errno)
{
  int r = translate_error_(error_code, default_error);
  return return_errno ? (r > 0 ? errno : r) : r;
}


/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void libgamma_x_randr_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  char* display = getenv("DISPLAY");
  this->crtc_information = CRTC_INFO_EDID
			 | CRTC_INFO_WIDTH_MM
			 | CRTC_INFO_HEIGHT_MM
			 | CRTC_INFO_WIDTH_MM_EDID
			 | CRTC_INFO_HEIGHT_MM_EDID
			 | CRTC_INFO_GAMMA_SIZE
			 | CRTC_INFO_GAMMA_DEPTH
			 | CRTC_INFO_SUBPIXEL_ORDER
			 | CRTC_INFO_CONNECTOR_NAME
			 | CRTC_INFO_CONNECTOR_TYPE
			 | CRTC_INFO_GAMMA;
  this->default_site_known = (display && *display) ? 1 : 0;
  this->multiple_sites = 1;
  this->multiple_partitions = 1;
  this->multiple_crtcs = 1;
  this->partitions_are_graphics_cards = 0;
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 0;
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  this->fixed_gamma_depth = 1;
  this->real = 1;
  this->fake = 0;
}


# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Waggregate-return"


/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. One the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int libgamma_x_randr_site_initialise(libgamma_site_state_t* restrict this,
				     char* restrict site)
{
  xcb_generic_error_t* error = NULL;
  xcb_connection_t* connection;
  xcb_randr_query_version_cookie_t cookie;
  xcb_randr_query_version_reply_t* reply;
  const xcb_setup_t* setup;
  xcb_screen_iterator_t iter;
  
  this->data = connection = xcb_connect(site, NULL);
  if (connection == NULL)
    return LIBGAMMA_NO_SUCH_SITE;
  
  cookie = xcb_randr_query_version(connection, RANDR_VERSION_MAJOR, RANDR_VERSION_MINOR);
  reply = xcb_randr_query_version_reply(connection, cookie, &error);
  
  if ((error != NULL) || (reply == NULL))
    {
      free(reply);
      xcb_disconnect(connection);
      if (error != NULL)
	return translate_error(error->error_code, LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED, 0);
      return LIBGAMMA_PROTOCOL_VERSION_QUERY_FAILED;
    }
  
  if ((reply->major_version != RANDR_VERSION_MAJOR) ||
      (reply->minor_version < RANDR_VERSION_MINOR))
    {
#ifdef DEBUG
      fprintf(stderr, "libgamma: RandR protocol version: %u.%u", reply->major_version, reply->minor_version);
#endif
      free(reply);
      xcb_disconnect(connection);
      return LIBGAMMA_PROTOCOL_VERSION_NOT_SUPPORTED;
    }
  
  free(reply);
  
  if ((setup = xcb_get_setup(connection)) == NULL)
    {
      xcb_disconnect(connection);
      return LIBGAMMA_LIST_PARTITIONS_FAILED;
    }
  iter = xcb_setup_roots_iterator(setup);
  this->partitions_available = (size_t)(iter.rem);
  
  return iter.rem < 0 ? LIBGAMMA_NEGATIVE_PARTITION_COUNT : 0;
}


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_x_randr_site_destroy(libgamma_site_state_t* restrict this)
{
  xcb_disconnect((xcb_connection_t*)(this->data));
}


/**
 * Restore the gamma ramps all CRTCS with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_x_randr_site_restore(libgamma_site_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_x_randr_partition_initialise(libgamma_partition_state_t* restrict this,
					  libgamma_site_state_t* restrict site, size_t partition)
{
  xcb_connection_t* restrict connection = site->data;
  const xcb_setup_t* setup = xcb_get_setup(connection);
  xcb_screen_t* screen = NULL;
  xcb_generic_error_t* error = NULL;
  xcb_screen_iterator_t iter;
  xcb_randr_get_screen_resources_current_cookie_t cookie;
  xcb_randr_get_screen_resources_current_reply_t* reply;
  xcb_randr_crtc_t* crtcs;
  size_t i;
  
  if (setup == NULL)
    return LIBGAMMA_LIST_PARTITIONS_FAILED;
  
  for (i = 0; iter.rem > 0; i++, xcb_screen_next(&iter))
    if (i == partition)
      {
	screen = iter.data;
	break;
      }
  if (iter.rem == 0)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  if (screen == NULL)
    return LIBGAMMA_NULL_PARTITION;
  
  cookie = xcb_randr_get_screen_resources_current(connection, screen->root);
  reply = xcb_randr_get_screen_resources_current_reply(connection, cookie, &error);
  if (error != NULL)
    return translate_error(error->error_code, LIBGAMMA_LIST_CRTCS_FAILED, 0);
  
  this->crtcs_available = reply->num_crtcs;
  crtcs = xcb_randr_get_screen_resources_current_crtcs(reply);
  /* Copy the CRTC:s, just so we do not have to keep the reply in memory. */
  this->data = malloc(reply->num_crtcs * sizeof(xcb_randr_crtc_t));
  if (this->data == NULL)
    {
      free(reply);
      return LIBGAMMA_ERRNO_SET;
    }
  memcpy(this->data, crtcs, reply->num_crtcs * sizeof(xcb_randr_crtc_t));
  free(reply);
  return 0;
}


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_x_randr_partition_destroy(libgamma_partition_state_t* restrict this)
{
  free(this->data);
}


/**
 * Restore the gamma ramps all CRTCS with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_x_randr_partition_restore(libgamma_partition_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The the index of the CRTC within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_x_randr_crtc_initialise(libgamma_crtc_state_t* restrict this,
				     libgamma_partition_state_t* restrict partition, size_t crtc)
{
  xcb_randr_crtc_t* crtc_ids = partition->data;
  if (crtc >= partition->crtcs_available)
    return LIBGAMMA_NO_SUCH_CRTC;
  this->data = crtc_ids + crtc;
  return 0;
}


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_x_randr_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_x_randr_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Get the gamma ramp size of a CRTC
 * 
 * @param   this  Instance of a data structure to fill with the information about the CRTC
 * @param   crtc  The state of the CRTC whose information should be read
 * @return        Non-zero on error
 */
static int get_gamma_ramp_size(libgamma_crtc_information_t* restrict out, libgamma_crtc_state_t* restrict crtc)
{
  xcb_connection_t* restrict connection = crtc->partition->site->data;
  xcb_randr_crtc_t* restrict crtc_id = crtc->data;
  xcb_randr_get_crtc_gamma_size_cookie_t cookie;
  xcb_randr_get_crtc_gamma_size_reply_t* reply;
  xcb_generic_error_t* error;
  
  out->gamma_size_error = 0;
  cookie = xcb_randr_get_crtc_gamma_size(connection, *crtc_id);
  reply = xcb_randr_get_crtc_gamma_size_reply(connection, cookie, &error);
  if (error != NULL)
    return out->gamma_size_error = translate_error(error->error_code, LIBGAMMA_GAMMA_RAMPS_SIZE_QUERY_FAILED, 1);
  if (reply->size < 2)
    out->gamma_size_error = LIBGAMMA_SINGLETON_GAMMA_RAMP;
  out->red_gamma_size = out->green_gamma_size = out->blue_gamma_size = reply->size;
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
static int read_output_data(libgamma_crtc_information_t* restrict out, xcb_randr_get_output_info_reply_t* restrict output)
{
  switch (output->connection)
    {
    case XCB_RANDR_CONNECTION_CONNECTED:
      out->active = 1;
      out->width_mm = output->mm_width;
      out->height_mm = output->mm_height;
#define __select(value)  \
  case XCB_RENDER_SUB_PIXEL_##value:  out->subpixel_order = LIBGAMMA_SUBPIXEL_ORDER_##value;  break
      switch (output->subpixel_order)
	{
	__select (UNKNOWN);
	__select (HORIZONTAL_RGB);
	__select (HORIZONTAL_BGR);
	__select (VERTICAL_RGB);
	__select (VERTICAL_BGR);
	__select (NONE);
	default:
	  out->subpixel_order_error = LIBGAMMA_SUBPIXEL_ORDER_NOT_RECOGNISED;
	  break;
	}
#undef __select
      return 0;
      
    case XCB_RANDR_CONNECTION_DISCONNECTED:
      out->active = 0;
      out->width_mm_error = LIBGAMMA_NOT_CONNECTED;
      out->height_mm_error = LIBGAMMA_NOT_CONNECTED;
      out->subpixel_order_error = LIBGAMMA_NOT_CONNECTED;
      return 0;
      
    default:
      out->active = 0;
      out->active_error = LIBGAMMA_STATE_UNKNOWN;
      out->width_mm_error = LIBGAMMA_NOT_CONNECTED;
      out->height_mm_error = LIBGAMMA_NOT_CONNECTED;
      out->subpixel_order_error = LIBGAMMA_NOT_CONNECTED;
      return -1;
    }
}


/**
 * Determine the connector type from the connector name
 * 
 * @param  this  The CRTC information to use and extend
 * @param        Non-zero on error
 */
static int get_connector_type(libgamma_crtc_information_t* restrict this)
{
  if ((this->connector_type_error = this->connector_name_error))
    return -1;
  
#define __select(name, type)                                           \
  if (strstr(this->connector_name, name "-") == this->connector_name)  \
    return this->connector_type = LIBGAMMA_CONNECTOR_TYPE_##type, 0
  
  __select ("None",         Unknown);
  __select ("VGA",          VGA);
  __select ("DVI-I",        DVII);
  __select ("DVI-D",        DVID);
  __select ("DVI-A",        DVIA);
  __select ("Composite",    Composite);
  __select ("S-Video",      SVIDEO);
  __select ("Component",    Component);
  __select ("LFP",          LFP);
  __select ("Proprietary",  Unknown);
  __select ("HDMI",         HDMI);
  __select ("DisplayPort",  DisplayPort);
  
#undef __select
  
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
static int get_output_name(libgamma_crtc_information_t* restrict out, xcb_randr_get_output_info_reply_t* restrict output)
{
  char* store;
  uint8_t* name;
  uint16_t length;
  size_t i;
  
  name = xcb_randr_get_output_info_name(output);
  length = output->name_len; /* There is no NUL-termination. */
  if (name == NULL)
    {
      out->connector_name_error = LIBGAMMA_REPLY_VALUE_EXTRACTION_FAILED;
      return -1;
    }
  
  store = malloc(((size_t)length + 1) * sizeof(char));
  if (store == NULL)
    {
      out->connector_name_error = errno;
      return -1;
    }
  
  /* char is guaranteed to be (u)int_least8_t, but it is only guaranteed to be (u)int8_t
   * on POSIX, so to be truly portable we will not assume that char is (u)int8_t. */
  for (i = 0; i < (size_t)length; i++)
    store[i] = (char)(name[i]);
  store[length] = '\0';
  
  return 0;
}


/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_x_randr_get_crtc_information(libgamma_crtc_information_t* restrict this,
					  libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)
  int e = 0;
  int free_edid;
  
  
  /* Wipe all error indicators. */
  memset(this, 0, sizeof(libgamma_crtc_information_t));
  
  /* We need to free the EDID after us if it is not explicitly requested.  */
  free_edid = (fields & CRTC_INFO_EDID) == 0;
  
  /* Figure out what fields we need to get the data for to get the data for other fields. */
  if ((fields & (CRTC_INFO_WIDTH_MM_EDID | CRTC_INFO_HEIGHT_MM_EDID | CRTC_INFO_GAMMA)))
    fields |= CRTC_INFO_EDID;
  if ((fields & CRTC_INFO_CONNECTOR_TYPE))
    fields |= CRTC_INFO_CONNECTOR_NAME;
  if ((fields & (CRTC_INFO_WIDTH_MM | CRTC_INFO_HEIGHT_MM | CRTC_INFO_SUBPIXEL_ORDER)))
    fields |= CRTC_INFO_ACTIVE;
  
  /* Jump if the output information is not required. */
  if ((fields & (CRTC_INFO_ACTIVE | CRTC_INFO_CONNECTOR_NAME)) == 0)
    goto cont;
  
  /* FIXME output */
  
  e |= get_output_name(this, output);
  if ((fields & CRTC_INFO_CONNECTOR_TYPE))
    e |= get_connector_type(this);
  e |= read_output_data(this, output);
  if ((fields & (CRTC_INFO_WIDTH_MM | CRTC_INFO_HEIGHT_MM)))
    e |= this->width_mm_error | this->height_mm_error;
  e |= (fields & CRTC_INFO_SUBPIXEL_ORDER) ? this->subpixel_order_error : 0;
  
  if ((fields & CRTC_INFO_EDID) == 0)
    goto cont;
  if (this->active == 0)
    {
      e |= this->edid_error = this->gamma_error = this->width_mm_edid_error
	 = this->height_mm_edid_error = LIBGAMMA_NOT_CONNECTED;
      goto cont;
    }
  e |= get_edid(this, output); /* FIXME */
  if (this->edid == NULL)
    {
      this->gamma_error = this->width_mm_edid_error = this->height_mm_edid_error = this->edid_error;
      goto cont;
    }
  if ((fields & (CRTC_INFO_WIDTH_MM_EDID | CRTC_INFO_HEIGHT_MM_EDID | CRTC_INFO_GAMMA)))
    e |= libgamma_parse_edid(this, fields);
  
 cont:
  e |= (fields & CRTC_INFO_GAMMA_SIZE) ? get_gamma_ramp_size(this, crtc) : 0;
  this->gamma_depth = 16;
  e |= this->gamma_support_error = _E(CRTC_INFO_GAMMA_SUPPORT);
  
  /* Free the EDID after us. */
  if (free_edid)
    {
      free(this->edid);
      this->edid = NULL;
    }
  
  free(output);
  return e ? -1 : 0;
#undef _E
}


/**
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_x_randr_crtc_get_gamma_ramps(libgamma_crtc_state_t* restrict this,
					  libgamma_gamma_ramps_t* restrict ramps)
{
  xcb_connection_t* restrict connection = this->partition->site->data;
  xcb_randr_get_crtc_gamma_cookie_t cookie;
  xcb_randr_get_crtc_gamma_reply_t* restrict reply;
  xcb_generic_error_t* error;
  uint16_t* restrict red;
  uint16_t* restrict green;
  uint16_t* restrict blue;
  
#ifdef DEBUG
  if ((ramps->red_size != ramps->green_size) ||
      (ramps->red_size != ramps->blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  
  cookie = xcb_randr_get_crtc_gamma(connection, *(xcb_randr_crtc_t*)(this->data));
  reply = xcb_randr_get_crtc_gamma_reply(connection, cookie, &error);
  
  if (error != NULL)
    return translate_error(error->error_code, LIBGAMMA_GAMMA_RAMP_READ_FAILED, 0);
  
  red   = xcb_randr_get_crtc_gamma_red(reply);
  green = xcb_randr_get_crtc_gamma_green(reply);
  blue  = xcb_randr_get_crtc_gamma_blue(reply);
  memcpy(ramps->red,   red,   ramps->red_size   * sizeof(uint16_t));
  memcpy(ramps->green, green, ramps->green_size * sizeof(uint16_t));
  memcpy(ramps->blue,  blue,  ramps->blue_size  * sizeof(uint16_t));
  
  free(reply);
  return 0;
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_x_randr_crtc_set_gamma_ramps(libgamma_crtc_state_t* restrict this,
					  libgamma_gamma_ramps_t ramps)
{
  xcb_connection_t* restrict connection = this->partition->site->data;
  xcb_void_cookie_t cookie;
  xcb_generic_error_t* restrict error;
#ifdef DEBUG
  if ((ramps.red_size != ramps.green_size) ||
      (ramps.red_size != ramps.blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  cookie = xcb_randr_set_crtc_gamma_checked(connection, *(xcb_randr_crtc_t*)(this->data),
					    (uint16_t)(ramps.red_size), ramps.red, ramps.green, ramps.blue);
  if ((error = xcb_request_check(connection, cookie)) != NULL)
    return translate_error(error->error_code, LIBGAMMA_GAMMA_RAMP_WRITE_FAILED, 0);
  return 0;
}


# pragma GCC diagnostic pop

