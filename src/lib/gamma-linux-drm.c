/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014, 2015  Mattias Andrée (maandree@member.fsf.org)
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
#ifndef HAVE_LIBGAMMA_METHOD_LINUX_DRM
# error Compiling gamma-linux-drm.c without HAVE_LIBGAMMA_METHOD_LINUX_DRM
#endif

#define _GNU_SOURCE

#include "gamma-linux-drm.h"

#include "libgamma-error.h"
#include "edid.h"

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <xf86drm.h>
#include <xf86drmMode.h>

#ifndef O_CLOEXEC
# define O_CLOEXEC  02000000
#endif
#ifndef NGROUPS_MAX
# define NGROUPS_MAX  65536
#endif
#ifndef PATH_MAX
# define PATH_MAX  4096
#endif



/**
 * Graphics card data for the Direct Rendering Manager adjustment method.
 */
typedef struct libgamma_drm_card_data
{
  /**
   * File descriptor for the connection to the graphics card.
   */
  int fd;
  
  /**
   * The graphics card's mode resources.
   */
  drmModeRes* res;
  
  /**
   * Resources for open connectors.
   */
  drmModeConnector** connectors;
  
  /**
   * Resources for open encoders.
   */
  drmModeEncoder** encoders;
  
} libgamma_drm_card_data_t;



/**
 * Return the capabilities of the adjustment method.
 * 
 * @param  this  The data structure to fill with the method's capabilities.
 */
void libgamma_linux_drm_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  /* Support for all information except gamma ramp support. */
  this->crtc_information = LIBGAMMA_CRTC_INFO_MACRO_EDID
			 | LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT
			 | LIBGAMMA_CRTC_INFO_MACRO_RAMP
			 | LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER
			 | LIBGAMMA_CRTC_INFO_ACTIVE
			 | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR;
  /* DRM supports multiple partitions and CRTC:s but not sites. */
  this->default_site_known = 1;
  this->multiple_sites = 0;
  this->multiple_partitions = 1;
  this->multiple_crtcs = 1;
  /* Partitions are graphics cards in DRM. */
  this->partitions_are_graphics_cards = 1;
  /* Linux does not have system restore capabilities. */
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 0;
  /* Gamma ramp sizes are identical but not fixed. */
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  /* Gamma ramp depths are fixed. */
  this->fixed_gamma_depth = 1;
  /* DRM is a real non-faked adjustment method */
  this->real = 1;
  this->fake = 0;
  /* Gamma ramp adjustments are persistent. */
  this->auto_restore = 0;
}


/**
 * Initialise an allocated site state.
 * 
 * @param   this    The site state to initialise.
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. One the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library.
 */
int libgamma_linux_drm_site_initialise(libgamma_site_state_t* restrict this,
				       char* restrict site)
{
  char pathname[PATH_MAX];
  struct stat _attr;
  
  if (site != NULL)
    return LIBGAMMA_NO_SUCH_SITE;
  
  /* Count the number of available graphics cards by
     `stat`:ing their existence in an API filesystem. */
  this->partitions_available = 0;
  for (;;)
    {
      /* Construct pathname of graphics card device. */
      snprintf(pathname, sizeof(pathname) / sizeof(char),
	       DRM_DEV_NAME, DRM_DIR_NAME, (int)(this->partitions_available));
      /* `stat` the graphics card's existence. */
      if (stat(pathname, &_attr))
	break;
      /* Move on to next graphics card. */
      if (this->partitions_available++ > INT_MAX)
	return LIBGAMMA_IMPOSSIBLE_AMOUNT;
    }
  return 0;
}


/**
 * Release all resources held by a site state.
 * 
 * @param  this  The site state.
 */
void libgamma_linux_drm_site_destroy(libgamma_site_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps all CRTC:s with a site to the system settings.
 * 
 * @param   this  The site state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_linux_drm_site_restore(libgamma_site_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}


/**
 * Figure out why `open` failed for a graphics card
 * 
 * @param   pathname  The pathname of the error card
 * @return            The error code to report
 */
static int figure_out_card_open_error(const char* pathname)
{
  gid_t supplemental_groups[NGROUPS_MAX];
  struct group* group;
  struct stat attr;
  int i, n;
  
  
  /* Check which the device exists. */
  if ((errno == ENXIO) || (errno == ENODEV))
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  
  /* If we did not get access permission, figure out why. */
  
  if (errno != EACCES)
    /* If we could not figure out what
       went wrong, just return the error
       we got. */
    return LIBGAMMA_ERRNO_SET;
  
#define __test(R, W) ((attr.st_mode & (R | W)) == (R | W))
      
  /* Get permission requirement for the file. */
  if (stat(pathname, &attr) < 0)
    return errno == EACCES ? LIBGAMMA_NO_SUCH_PARTITION : LIBGAMMA_ERRNO_SET;
  
  /* Test owner's, group's and others' permissions. */
  if ((attr.st_uid == geteuid() && __test(S_IRUSR, S_IWUSR)) ||
      (attr.st_gid == getegid() && __test(S_IRGRP, S_IWGRP)) ||
      __test(S_IROTH, S_IWOTH))
    return LIBGAMMA_DEVICE_ACCESS_FAILED;
  
  /* The group should be "video", but perhaps
     it is "root" to restrict users. */
  if (attr.st_gid == 0 /* root group */ || __test(S_IRGRP, S_IWGRP))
    return LIBGAMMA_DEVICE_RESTRICTED;
  
  
  /* Get the user's supplemental group membership list. */
  if ((n = getgroups(NGROUPS_MAX, supplemental_groups)) < 0)
    return LIBGAMMA_ERRNO_SET;
  
  /* Test whether any of the supplemental
     group should be satisfactory. */
  for (i = 0; i < n; i++)
    if (supplemental_groups[i] == attr.st_gid)
      break;
  
  /* If one of the supplemental groups
     should be satisfactory, then we
     do not know anything more than
     that access failed. */
  if (i != n)
    return LIBGAMMA_DEVICE_ACCESS_FAILED;
  
  /* Otherwise, try to get the name of
     the group that is required and
     report the missing group membership. */ 
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
  /* Thread-safe. */
  {
    static __thread char buf[1024]; /* My output of `sysconf(_SC_GETGR_R_SIZE_MAX)`. */
    struct group _grp;
    
    errno = getgrgid_r(attr.st_gid, &_grp, buf, sizeof(buf) / sizeof(char), &group);
    if (errno == ERANGE)
      {
	/* The lenght of the group's name is absurdly long, degrade to thread-unsafe. */
	errno = 0;
	group = getgrgid(attr.st_gid);
      }
    else if (errno)
      return LIBGAMMA_ERRNO_SET;
  }
#else
# ifdef __GCC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wcpp"
#  warning figure_out_card_open_error is not thread-safe.
#  pragma GCC diagnostic pop
# endif
  /* Not thread-safe. */
  errno = 0;
  group = getgrgid(attr.st_gid);
#endif
  
  libgamma_group_gid = attr.st_gid;
  libgamma_group_name = group != NULL ? group->gr_name : NULL;
  return LIBGAMMA_DEVICE_REQUIRE_GROUP;
#undef __test
}


/**
 * Initialise an allocated partition state.
 * 
 * @param   this       The partition state to initialise.
 * @param   site       The site state for the site that the partition belongs to.
 * @param   partition  The the index of the partition within the site.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_linux_drm_partition_initialise(libgamma_partition_state_t* restrict this,
					    libgamma_site_state_t* restrict site, size_t partition)
{
  int rc = 0;
  libgamma_drm_card_data_t* restrict data;
  char pathname[PATH_MAX];
  
  (void) site;
  
  /* Check for partition index overflow. */
  if (partition > INT_MAX)
    return LIBGAMMA_NO_SUCH_PARTITION;
  
  /* Allocate and initialise graphics card data.  */
  this->data = NULL;
  data = malloc(sizeof(libgamma_drm_card_data_t));
  if (data == NULL)
    return LIBGAMMA_ERRNO_SET;
  data->fd = -1;
  data->res = NULL;
  data->encoders = NULL;
  data->connectors = NULL;
  
  /* Get the pathname for the graphics card. */
  snprintf(pathname, sizeof(pathname) / sizeof(char),
	   DRM_DEV_NAME, DRM_DIR_NAME, (int)partition);
  
  /* Acquire access to the graphics card. */
  data->fd = open(pathname, O_RDWR | O_CLOEXEC);
  if (data->fd < 0)
    {
      rc = figure_out_card_open_error(pathname);
      goto fail_data;
    }
  
  /* Acquire mode resources. */
  data->res = drmModeGetResources(data->fd);
  if (data->res == NULL)
    {
      rc = LIBGAMMA_ACQUIRING_MODE_RESOURCES_FAILED;
      goto fail_fd;
    }
  
  /* Get the number of CRTC:s that are available in the partition. */
  if (data->res->count_crtcs < 0)
    {
      rc = LIBGAMMA_NEGATIVE_CRTC_COUNT;
      goto fail_res;
    }
  this->crtcs_available = (size_t)(data->res->count_crtcs);
  this->data = data;
  return 0;
  
 fail_res:   drmModeFreeResources(data->res);
 fail_fd:    close(data->fd);
 fail_data:  free(data);
  return rc;
}


/**
 * Release all connectors and encoders.
 * 
 * @param  this  The graphics card data.
 */
static void release_connectors_and_encoders(libgamma_drm_card_data_t* restrict this)
{
  size_t i, n;
  /* Release individual encoders. */
  if (this->encoders != NULL)
    for (i = 0, n = (size_t)(this->res->count_connectors); i < n; i++)
      if (this->encoders[i] != NULL)
	drmModeFreeEncoder(this->encoders[i]);
  /* Release encoder array. */
  free(this->encoders);
  this->encoders = NULL;
  
  /* Release individual connectors. */
  if (this->connectors != NULL)
    for (i = 0, n = (size_t)(this->res->count_connectors); i < n; i++)
      if (this->connectors[i] != NULL)
	drmModeFreeConnector(this->connectors[i]);
  /* Release connector array. */
  free(this->connectors);
  this->connectors = NULL;
}


/**
 * Release all resources held by a partition state.
 * 
 * @param  this  The partition state.
 */
void libgamma_linux_drm_partition_destroy(libgamma_partition_state_t* restrict this)
{
  libgamma_drm_card_data_t* restrict data = this->data;
  release_connectors_and_encoders(data);
  if (data->res != NULL)  drmModeFreeResources(data->res);
  if (data->fd >= 0)      close(data->fd);
  free(data);
}


/**
 * Restore the gamma ramps all CRTC:s with a partition to the system settings.
 * 
 * @param   this  The partition state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_linux_drm_partition_restore(libgamma_partition_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}



/**
 * Initialise an allocated CRTC state.
 * 
 * @param   this       The CRTC state to initialise.
 * @param   partition  The partition state for the partition that the CRTC belongs to.
 * @param   crtc       The the index of the CRTC within the site.
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library.
 */
int libgamma_linux_drm_crtc_initialise(libgamma_crtc_state_t* restrict this,
				       libgamma_partition_state_t* restrict partition, size_t crtc)
{
  libgamma_drm_card_data_t* restrict card = partition->data;
  
  if (crtc >= partition->crtcs_available)
    return LIBGAMMA_NO_SUCH_CRTC;
  this->data = (void*)(size_t)(card->res->crtcs[crtc]);
  return 0;
}


/**
 * Release all resources held by a CRTC state.
 * 
 * @param  this  The CRTC state.
 */
void libgamma_linux_drm_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC.
 * 
 * @param   this  The CRTC state.
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library.
 */
int libgamma_linux_drm_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}


/**
 * Find the connector that a CRTC belongs to.
 * 
 * @param   this   The CRTC state.
 * @param   error  Output of the error value to store of error report
 *                 fields for data that requires the connector.
'* @return         The CRTC's conncetor, `NULL` on error.
 */
static drmModeConnector* find_connector(libgamma_crtc_state_t* restrict this, int* restrict error)
{
  uint32_t crtc_id = (uint32_t)(size_t)(this->data);
  libgamma_drm_card_data_t* restrict card = this->partition->data;
  size_t i, n = (size_t)(card->res->count_connectors);
  /* Open connectors and encoders if not already opened. */
  if (card->connectors == NULL)
    {
      /* Allocate connector and encoder arrays.
	 We use `calloc` so all non-loaded elements are `NULL` after an error. */
      if ((card->connectors = calloc(n, sizeof(drmModeConnector*))) == NULL)  goto fail;
      if ((card->encoders   = calloc(n, sizeof(drmModeEncoder*)))   == NULL)  goto fail;
      /* Fill connector and encoder arrays. */
      for (i = 0; i < n; i++)
	{
	  /* Get connector, */
	  if ((card->connectors[i] = drmModeGetConnector(card->fd, card->res->connectors[i])) == NULL)
	    goto fail;
	  /* Get encoder if the connector is enabled.
	     If it is disabled it will not have an
	     encoder, which is indicated by the
	     encoder ID being 0. In such case, leave
	     the encoder to be `NULL`. */
	  if ((card->connectors[i]->encoder_id != 0) &&
	      ((card->encoders[i] = drmModeGetEncoder(card->fd, card->connectors[i]->encoder_id)) == NULL))
	    goto fail;
	}
    }
  /* No error has occurred yet. */
  *error = 0;
  /* Find connector. */
  for (i = 0; i < n; i++)
    if ((card->encoders[i] != NULL) && (card->encoders[i]->crtc_id == crtc_id))
      return card->connectors[i];
  /* We did not find the connector. */
  *error = LIBGAMMA_CONNECTOR_UNKNOWN;
  return NULL;
  
 fail:
  /* Report the error that got us here, release
     resouces and exit with `NULL` for failure. */
  *error = errno;
  release_connectors_and_encoders(card);
  return NULL;
}


/**
 * Get the size of the gamma ramps for a CRTC.
 * 
 * @param   out   Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc  The state of the CRTC whose information should be read.
 * @return        The value stored in `out->gamma_size_error`.
 */
static int get_gamma_ramp_size(libgamma_crtc_information_t* restrict out, const libgamma_crtc_state_t* restrict crtc)
{
  libgamma_drm_card_data_t* restrict card = crtc->partition->data;
  uint32_t crtc_id = card->res->crtcs[crtc->crtc];
  drmModeCrtc* restrict crtc_info;
  /* Get CRTC information. */
  errno = 0;
  crtc_info = drmModeGetCrtc(card->fd, crtc_id);
  out->gamma_size_error = crtc_info == NULL ? errno : 0;
  /* Get gamma ramp size. */
  if (out->gamma_size_error == 0)
    {
      /* Store gamma ramp size. */
      out->red_gamma_size = out->green_gamma_size = out->blue_gamma_size = (size_t)(crtc_info->gamma_size);
      /* Sanity check gamma ramp size. */
      out->gamma_size_error = crtc_info->gamma_size < 2 ? LIBGAMMA_SINGLETON_GAMMA_RAMP : 0;
      /* Release CRTC information. */
      drmModeFreeCrtc(crtc_info);
    }
  return out->gamma_size_error;
}


/**
 * Get the a monitor's subpixel order
 * 
 * @param  out        Instance of a data structure to fill with the information about the CRTC.
 * @param  connector  The connector.
 */
static void get_subpixel_order(libgamma_crtc_information_t* restrict out,
			       const drmModeConnector* restrict connector)
{
#define __select(value)					    \
  case DRM_MODE_SUBPIXEL_##value:			    \
    out->subpixel_order = LIBGAMMA_SUBPIXEL_ORDER_##value;  \
    break
  
  switch (connector->subpixel)
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
}


/**
 * Get a connector's type
 * 
 * @param  out                  Instance of a data structure to fill with the information about the CRTC.
 * @param  connector            The connector.
 * @param  connector_name_base  Output for the basename of the connector.
 */
static void get_connector_type(libgamma_crtc_information_t* restrict out,
			       const drmModeConnector* restrict connector,
			       const char** restrict connector_name_base)
{
#define __select(type, name)				   \
  case DRM_MODE_CONNECTOR_##type:			   \
    out->connector_type = LIBGAMMA_CONNECTOR_TYPE_##type;  \
    *connector_name_base = name;			   \
    break
  
  /* These may not have been included by <xf86drmMode.h>,
     but they should be available. Becuase we define them
     outself, it is best to test them last. */
#ifndef DRM_MODE_CONNECTOR_VIRTUAL
# define DRM_MODE_CONNECTOR_VIRTUAL  15
#endif
#ifndef DRM_MODE_CONNECTOR_DSI
# define DRM_MODE_CONNECTOR_DSI  16
#endif
  
  /* Translate connector type from DRM to libgamma
     and store connector basename. */
  switch (connector->connector_type)
    {
    __select (Unknown,      "Unknown"  );
    __select (VGA,          "VGA"      );
    __select (DVII,         "DVI-I"    );
    __select (DVID,         "DVI-D"    );
    __select (DVIA,         "DVI-A"    );
    __select (Composite,    "Composite");
    __select (SVIDEO,       "SVIDEO"   );
    __select (LVDS,         "LVDS"     );
    __select (Component,    "Component");
    __select (9PinDIN,      "DIN"      );
    __select (DisplayPort,  "DP"       );
    __select (HDMIA,        "HDMI-A"   );
    __select (HDMIB,        "HDMI-B"   );
    __select (TV,           "TV"       );
    __select (eDP,          "eDP"      );
    __select (VIRTUAL,      "VIRTUAL"  );
    __select (DSI,          "DSI"      );
    default:
      out->connector_type_error = LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED;
      out->connector_name_error = LIBGAMMA_CONNECTOR_TYPE_NOT_RECOGNISED;
      break;
    }
  
#undef __select
}


/**
 * Read information from the CRTC's conncetor.
 * 
 * @param   crtc       The state of the CRTC whose information should be read.
 * @param   out        Instance of a data structure to fill with the information about the CRTC.
 * @param   connector  The CRTC's connector.
 * @param   fields     OR:ed identifiers for the information about the CRTC that should be read.
 * @return             Non-zero if at least on error occured.
 */
static int read_connector_data(libgamma_crtc_state_t* restrict crtc, libgamma_crtc_information_t* restrict out,
			       const drmModeConnector* restrict connector, int32_t fields)
{
  const char* connector_name_base = NULL;
  
  /* Get some information that does not require too much work. */
  if ((fields & (LIBGAMMA_CRTC_INFO_MACRO_ACTIVE | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR)))
    {
      /* Get whether or not a monitor is plugged in. */
      out->active = connector->connection == DRM_MODE_CONNECTED;
      out->active_error = connector->connection == DRM_MODE_UNKNOWNCONNECTION ? LIBGAMMA_STATE_UNKNOWN : 0;
      if (out->active == 0)
	{
	  if ((fields & (LIBGAMMA_CRTC_INFO_MACRO_VIEWPORT | LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER)))
	    out->width_mm_error = out->height_mm_error = out->subpixel_order_error = LIBGAMMA_NOT_CONNECTED;
	  goto not_connected;
	}
      
      /* Get viewport dimension. */
      out->width_mm = connector->mmWidth;
      out->height_mm = connector->mmHeight;
      
      /* Get subpixel order. */
      get_subpixel_order(out, connector);
      
    not_connected:
      
      /* Get connector type. */
      get_connector_type(out, connector, &connector_name_base);
    }
  
  /* Get the connector's name. */
  if ((fields & LIBGAMMA_CRTC_INFO_CONNECTOR_NAME) && (out->connector_name_error == 0))
    {
      libgamma_drm_card_data_t* restrict card = crtc->partition->data;
      uint32_t type = connector->connector_type;
      size_t i, n = (size_t)(card->res->count_connectors), c = 0;
      
      /* Allocate memory for the name of the connector. */
      out->connector_name = malloc((strlen(connector_name_base) + 12) * sizeof(char));
      if (out->connector_name == NULL)
	return out->connector_name_error = errno;
      
      /* Get the number of connectors with the same type on the same graphics card. */
      for (i = 0; (i < n) && (card->connectors[i] != connector); i++)
	if (card->connectors[i]->connector_type == type)
	  c++;
      
      /* Construct and store connect name that is unique to the graphics card. */
      sprintf(out->connector_name, "%s-%" PRIu32, connector_name_base, (uint32_t)(c + 1));
    }
  
  /* Did something go wrong? */
  return out->subpixel_order_error | out->active_error | out->connector_name_error;
}


/**
 * Get the extended display identification data for a monitor.
 * 
 * @param   crtc       The CRTC state.
 * @param   out        Instance of a data structure to fill with the information about the CRTC.
 * @param   connector  The CRTC's connector.
 * @reutnr             Non-zero on error.
 */
static int get_edid(libgamma_crtc_state_t* restrict crtc,
		    libgamma_crtc_information_t* restrict out, drmModeConnector* connector)
{
  libgamma_drm_card_data_t* restrict card = crtc->partition->data;
  int prop_n = connector->count_props;
  int prop_i;
  drmModePropertyRes* restrict prop;
  drmModePropertyBlobRes* restrict blob;
  
  /* Test all properies on the connector. */
  for (prop_i = 0; prop_i < prop_n; prop_i++)
    {
      /* Get output property, */
      if ((prop = drmModeGetProperty(card->fd, connector->props[prop_i])) == NULL)
	continue;
      /* Is this property the EDID? */
      if (!strcmp(prop->name, "EDID"))
	{
	  /* Get the property value. */
	  if ((blob = drmModeGetPropertyBlob(card->fd, (uint32_t)(connector->prop_values[prop_i]))) == NULL)
	    return drmModeFreeProperty(prop), out->edid_error = LIBGAMMA_PROPERTY_VALUE_QUERY_FAILED;
	  if (blob->data != NULL)
	    {
	      /* Get and store the length of the EDID. */
	      out->edid_length = blob->length;
	      /* Allocate memory for a copy of the EDID that is under our memory control. */
	      if ((out->edid = malloc(out->edid_length * sizeof(unsigned char))) == NULL)
		out->edid_error = errno;
	      else
		/* Copy the EDID so we can free resources that got us here. */
		memcpy(out->edid, blob->data, (size_t)(out->edid_length) * sizeof(char));
	      /* Free the propriety value and the propery. */
	      drmModeFreePropertyBlob(blob);
	      drmModeFreeProperty(prop);
	      /* Were we successful? */
	      return out->edid == NULL;
	    }
	  /* Free the propriety value. */
	  drmModeFreePropertyBlob(blob);
	}
      /* Free the propriety. */
      drmModeFreeProperty(prop);
    }
  /* If we get here, we did not find a EDID. */
  return out->edid_error = LIBGAMMA_EDID_NOT_FOUND;
}


/**
 * Read information about a CRTC.
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC.
 * @param   crtc    The state of the CRTC whose information should be read.
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read.
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_linux_drm_get_crtc_information(libgamma_crtc_information_t* restrict this,
					    libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)
  int e = 0;
  drmModeConnector* restrict connector;
  int require_connector;
  int free_edid;
  int error;
  
  
  /* Wipe all error indicators. */
  memset(this, 0, sizeof(libgamma_crtc_information_t));
  
  /* We need to free the EDID after us if it is not explicitly requested.  */
  free_edid = (fields & LIBGAMMA_CRTC_INFO_EDID) == 0;
  
  /* Figure out whether we require the connector to get all information we want. */
  require_connector = fields & (LIBGAMMA_CRTC_INFO_MACRO_ACTIVE | LIBGAMMA_CRTC_INFO_MACRO_CONNECTOR);
  
  /* If we are not interested in the connector or monitor, jump. */
  if (require_connector == 0)
    goto cont;
  /* Find connector. */
  if ((connector = find_connector(crtc, &error)) == NULL)
    {
      perror("connector not found");
      abort();
      /* Store reported error in affected fields. */
      e |= this->width_mm_error       = this->height_mm_error
	 = this->connector_type_error = this->subpixel_order_error
	 = this->active_error         = this->connector_name_error
	 = this->edid_error           = this->gamma_error
	 = this->width_mm_edid_error  = this->height_mm_edid_error = error;
      goto cont;
    }
  
  /* Read connector data and monitor data, excluding EDID.. */
  e |= read_connector_data(crtc, this, connector, fields);
  
  /* If we do not want any EDID information, jump. */
  if ((fields & LIBGAMMA_CRTC_INFO_MACRO_EDID) == 0)
    goto cont;
  /* If there is not monitor that report error in EDID related fields. */
  if (this->active_error || (this->active == 0))
    {
      e |= this->edid_error = this->gamma_error
	 = this->width_mm_edid_error = this->height_mm_edid_error
	 = LIBGAMMA_NOT_CONNECTED;
      goto cont;
    }
  /* Get EDID. */
  e |= get_edid(crtc, this, connector);
  if (this->edid == NULL)
    {
      this->gamma_error = this->width_mm_edid_error = this->height_mm_edid_error = this->edid_error;
      goto cont;
    }
  /* Parse EDID. */
  if ((fields & (LIBGAMMA_CRTC_INFO_MACRO_EDID ^ LIBGAMMA_CRTC_INFO_EDID)))
    e |= libgamma_parse_edid(this, fields);
  
 cont:
  /* Get gamma ramp size. */
  e |= (fields & LIBGAMMA_CRTC_INFO_GAMMA_SIZE) ? get_gamma_ramp_size(this, crtc) : 0;
  /* Store gamma ramp depth. */
  this->gamma_depth = 16;
  /* DRM does not support quering gamma ramp support. */
  e |= this->gamma_support_error = _E(LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT);
  
  /* Free the EDID after us. */
  if (free_edid)
    {
      free(this->edid);
      this->edid = NULL;
    }
  
  return e ? -1 : 0;
#undef _E
}


/**
 * Get the current gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to fill with the current values.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_linux_drm_crtc_get_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					      libgamma_gamma_ramps16_t* restrict ramps)
{
  libgamma_drm_card_data_t* restrict card = this->partition->data;
  int r;
#ifdef DEBUG
  /* Gamma ramp sizes are identical but not fixed. */
  if ((ramps->red_size != ramps->green_size) ||
      (ramps->red_size != ramps->blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  /* Read current gamma ramps. */
  r = drmModeCrtcGetGamma(card->fd, (uint32_t)(size_t)(this->data), (uint32_t)(ramps->red_size),
			  ramps->red, ramps->green, ramps->blue);
  return r ? LIBGAMMA_GAMMA_RAMP_READ_FAILED : 0;
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version.
 * 
 * @param   this   The CRTC state.
 * @param   ramps  The gamma ramps to apply.
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library.
 */
int libgamma_linux_drm_crtc_set_gamma_ramps16(libgamma_crtc_state_t* restrict this,
					      libgamma_gamma_ramps16_t ramps)
{
  libgamma_drm_card_data_t* restrict card = this->partition->data;
  int r;
#ifdef DEBUG
  /* Gamma ramp sizes are identical but not fixed. */
  if ((ramps.red_size != ramps.green_size) ||
      (ramps.red_size != ramps.blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  
  /* Apply gamma ramps. */
  r = drmModeCrtcSetGamma(card->fd, (uint32_t)(size_t)(this->data),
			  (uint32_t)(ramps.red_size), ramps.red, ramps.green, ramps.blue);
  /* Check for errors. */
  if (r)
    switch (errno)
      {
      case EACCES:
      case EAGAIN:
      case EIO:
	/* Permission denied errors must be ignored, because we do not
	 * have permission to do this while a display server is active.
	 * We are also checking for some other error codes just in case. */
      case EBUSY:
      case EINPROGRESS:
	/* It is hard to find documentation for DRM (in fact all of this is
	 * just based on the functions names and some testing,) perhaps we
	 * could get this if we are updating to fast. */
	break;
      case EBADF:
      case ENODEV:
      case ENXIO:
	/* XXX: I have not actually tested removing my graphics card or,
	 *      monitor but I imagine either of these is what would happen. */
	return LIBGAMMA_GRAPHICS_CARD_REMOVED;

      default:
	return LIBGAMMA_ERRNO_SET;
      }
  return 0;
}

