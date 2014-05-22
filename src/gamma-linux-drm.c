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
#ifndef HAVE_GAMMA_METHOD_LINUX_DRM
# error Compiling gamma-linux-drm.c without HAVE_GAMMA_METHOD_LINUX_DRM
#endif

#define _GNU_SOURCE

#include "gamma-linux-drm.h"

#include "libgamma-error.h"

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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
 * Graphics card data for the Direct Rendering Manager adjustment method
 */
typedef struct libgamma_drm_card_data
{
  /**
   * File descriptor for the connection to the graphics card
   */
  int fd;
  
  /**
   * The graphics card's mode resources
   */
  drmModeRes* res;
  
} libgamma_drm_card_data_t;



/**
 * Return the capabilities of the adjustment method
 * 
 * @param  this  The data structure to fill with the method's capabilities
 */
void libgamma_linux_drm_method_capabilities(libgamma_method_capabilities_t* restrict this)
{
  this->crtc_information = CRTC_INFO_EDID
			 | CRTC_INFO_WIDTH_MM
			 | CRTC_INFO_HEIGHT_MM
			 | CRTC_INFO_WIDTH_MM_EDID
			 | CRTC_INFO_HEIGHT_MM_EDID
			 | CRTC_INFO_GAMMA_SIZE
			 | CRTC_INFO_GAMMA_DEPTH
			 | CRTC_INFO_SUBPIXEL_ORDER
			 | CRTC_INFO_ACTIVE
			 | CRTC_INFO_CONNECTOR_NAME
			 | CRTC_INFO_CONNECTOR_TYPE
			 | CRTC_INFO_GAMMA;
  this->default_site_known = 1;
  this->multiple_sites = 0;
  this->multiple_partitions = 1;
  this->multiple_crtcs = 1;
  this->partitions_are_graphics_cards = 1;
  this->site_restore = 0;
  this->partition_restore = 0;
  this->crtc_restore = 0;
  this->identical_gamma_sizes = 1;
  this->fixed_gamma_size = 0;
  this->fixed_gamma_depth = 1;
  this->real = 1;
  this->fake = 0;
}


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
int libgamma_linux_drm_site_initialise(libgamma_site_state_t* restrict this,
				       char* restrict site)
{
  char pathname[PATH_MAX];
  struct stat _attr;
  
  if (site != NULL)
    return LIBGAMMA_NO_SUCH_SITE;
  
  /* Count the number of available graphics cards by
   * stat:ing there existence in an API filesystem. */
  this->partitions_available = 0;
  for (;;)
    {
      snprintf(pathname, sizeof(pathname) / sizeof(char),
	       DRM_DEV_NAME, DRM_DIR_NAME, (int)(this->partitions_available));
      if (stat(pathname, &_attr))
	break;
      if (this->partitions_available++ > INT_MAX)
	return LIBGAMMA_IMPOSSIBLE_AMOUNT;
    }
  return 0;
}


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_linux_drm_site_destroy(libgamma_site_state_t* restrict this)
{
  (void) this;
}


/**
 * Restore the gamma ramps all CRTCS with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_linux_drm_site_restore(libgamma_site_state_t* restrict this)
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
int libgamma_linux_drm_partition_initialise(libgamma_partition_state_t* restrict this,
					    libgamma_site_state_t* restrict site, size_t partition)
{ /* FIXME: This function is too large. */
  int rc = 0;
  libgamma_drm_card_data_t* data;
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
  
  /* Get the pathname for the graphics card. */
  snprintf(pathname, sizeof(pathname) / sizeof(char),
	   DRM_DEV_NAME, DRM_DIR_NAME, (int)partition);
  
  /* Acquire access to the graphics card. */
  data->fd = open(pathname, O_RDWR | O_CLOEXEC);
  if (data->fd < 0)
    {
      if ((errno == ENXIO) || (errno == ENODEV))
	rc = LIBGAMMA_NO_SUCH_PARTITION;
      else if (errno == EACCES)
	{
	  struct stat attr;
	  int r;
	  
	  r = stat(pathname, &attr);
	  rc = LIBGAMMA_NO_SUCH_PARTITION;

#define __test(R, W) ((attr.st_mode & (R | W)) == (R | W))
	  if (r)
	    rc = errno == EACCES ? LIBGAMMA_NO_SUCH_PARTITION : LIBGAMMA_ERRNO_SET;
	  else if ((attr.st_uid == geteuid() && __test(S_IRUSR, S_IWUSR)) ||
		   (attr.st_gid == getegid() && __test(S_IRGRP, S_IWGRP)) ||
		   __test(S_IROTH, S_IWOTH))
	    rc = LIBGAMMA_DEVICE_ACCESS_FAILED;
	  else if (attr.st_gid == 0 /* root group */ || __test(S_IRGRP, S_IWGRP))
	    rc = LIBGAMMA_DEVICE_RESTRICTED;
	  else
	    {
	      gid_t supplemental_groups[NGROUPS_MAX];
	      struct group* group;
	      int i, n;
	      
	      n = getgroups(NGROUPS_MAX, supplemental_groups);
	      if (n < 0)
		{
		  rc = LIBGAMMA_ERRNO_SET;
		  goto fail_data;
		}
	      
	      for (i = 0; i < n; i++)
		if (supplemental_groups[i] == attr.st_gid)
		  break;
	      
	      if (i != n)
		{
		  rc = LIBGAMMA_DEVICE_ACCESS_FAILED;
		  goto fail_data;
		}
	      
	      rc = LIBGAMMA_DEVICE_REQUIRE_GROUP;
	      errno = 0;
	      group = getgrgid(attr.st_gid); /* TODO: Not thread-safe. */
	      libgamma_group_gid = attr.st_gid;
	      libgamma_group_name = group != NULL ? group->gr_name : NULL;
	    }
#undef __test
	}
      else
	rc = LIBGAMMA_ERRNO_SET;
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
  return 0;
  
 fail_res:
  drmModeFreeResources(data->res);
 fail_fd:
  close(data->fd);
 fail_data:
  free(data);
  return rc;
}


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_linux_drm_partition_destroy(libgamma_partition_state_t* restrict this)
{
  libgamma_drm_card_data_t* data = this->data;
  if (data->res != NULL)
    drmModeFreeResources(data->res);
  if (data->fd >= 0)
    close(data->fd);
  free(data);
}


/**
 * Restore the gamma ramps all CRTCS with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_linux_drm_partition_restore(libgamma_partition_state_t* restrict this)
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
int libgamma_linux_drm_crtc_initialise(libgamma_crtc_state_t* restrict this,
				       libgamma_partition_state_t* restrict partition, size_t crtc)
{
  libgamma_drm_card_data_t* card;
  uint32_t crtc_id;
  
  if (crtc >= partition->crtcs_available)
    return LIBGAMMA_NO_SUCH_CRTC;
  
  card = partition->data;
  crtc_id = card->res->crtcs[crtc];
  this->data = (void*)(size_t)crtc_id;
  return 0;
}


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_linux_drm_crtc_destroy(libgamma_crtc_state_t* restrict this)
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
int libgamma_linux_drm_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  (void) this;
  return errno = ENOTSUP, LIBGAMMA_ERRNO_SET;
}


/**
 * Get the size of the gamma ramps for a CRTC
 * 
 * @param   this  Instance of a data structure to fill with the information about the CRTC
 * @param   crtc  The state of the CRTC whose information should be read
 * @return        The value stored in `this->gamma_size_error`
 */
static int get_gamma_ramp_size(libgamma_crtc_information_t* restrict this, libgamma_crtc_state_t* restrict crtc)
{
  libgamma_drm_card_data_t* card = crtc->partition->data;
  uint32_t crtc_id = card->res->crtcs[crtc->crtc];
  drmModeCrtc* crtc_info;
  errno = 0;
  crtc_info = drmModeGetCrtc(card->fd, crtc_id);
  this->gamma_size_error = crtc_info == NULL ? errno : 0;
  if (this->gamma_size_error == 0)
    {
      this->red_gamma_size = (size_t)(crtc_info->gamma_size);
      this->green_gamma_size = (size_t)(crtc_info->gamma_size);
      this->blue_gamma_size = (size_t)(crtc_info->gamma_size);
      this->gamma_size_error = crtc_info->gamma_size < 2 ? LIBGAMMA_SINGLETON_GAMMA_RAMP : 0;
      drmModeFreeCrtc(crtc_info);
    }
  return this->gamma_size_error;
}


static int read_connector_data(libgamma_crtc_information_t* restrict this,
			       drmModeConnector* connector, int32_t fields)
{
  if (connector == NULL)
    {
      this->width_mm_error = this->height_mm_error = this->connector_type = this->active =
	this->active_error = this->connector_name = LIBGAMMA_CONNECTOR_UNKNOWN;
      return LIBGAMMA_CONNECTOR_UNKNOWN;
    }
  
  
  if ((fields & (CRTC_INFO_WIDTH_MM | CRTC_INFO_HEIGHT_MM | CRTC_INFO_CONNECTOR_TYPE | CRTC_INFO_ACTIVE)))
    {
      this->width_mm = connector->mmWidth;
      this->height_mm = connector->mmHeight;
      this->connector_type = (int)(connector->connector_type); /* TODO: needs abstraction */
      this->active = connector->connection == DRM_MODE_CONNECTED;
      this->active_error = connector->connection == DRM_MODE_UNKNOWNCONNECTION ? LIBGAMMA_STATE_UNKNOWN : 0;
    }
  
  if ((fields & CRTC_INFO_CONNECTOR_NAME))
    {
      static const char* TYPE_NAMES[] = /* FIXME: What names does Linux itself use? */
	{
	  "Unknown", "VGA", "DVII", "DVID", "DVIA", "Composite", "SVIDEO", "LVDS", "Component",
	  "9PinDIN", "DisplayPort", "HDMIA", "HDMIB", "TV", "eDP", "VIRTUAL", "DSI"
	};
      this->connector_name = (size_t)(this->connector_type) < sizeof(TYPE_NAMES) / sizeof(char*)
	? TYPE_NAMES[(size_t)(this->connector_type)] : "Unrecognised" /*TODO:error*/;
      /* FIXME : add index */
    }
  
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
int libgamma_linux_drm_get_crtc_information(libgamma_crtc_information_t* restrict this,
					    libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#define _E(FIELD)  ((fields & FIELD) ? LIBGAMMA_CRTC_INFO_NOT_SUPPORTED : 0)
  int e = 0;
  int require_connector;
  int free_edid;
  drmModeConnector* connector;
  
  /* Wipe all error indicators. */
  memset(this, 0, sizeof(libgamma_crtc_information_t));
  
  /* We need to free the EDID after us if it is not explicitly requested.  */
  free_edid = (fields & CRTC_INFO_EDID) == 0;
  
  /* Figure out what fields we need to get the data for to get the data for other fields. */
  if ((fields & (CRTC_INFO_WIDTH_MM_EDID | CRTC_INFO_HEIGHT_MM_EDID | CRTC_INFO_GAMMA)))
    fields |= CRTC_INFO_EDID;
  if ((fields & CRTC_INFO_CONNECTOR_NAME))
    fields |= CRTC_INFO_CONNECTOR_TYPE;
  
  /* Figure out whether we require the connector to get all information we want. */
  require_connector = fields & (CRTC_INFO_WIDTH_MM | CRTC_INFO_HEIGHT_MM |
				CRTC_INFO_SUBPIXEL_ORDER/*(?)*/ | CRTC_INFO_CONNECTOR_TYPE);
  
  e |= this->edid_error = _E(CRTC_INFO_EDID); /* TODO */
  e |= this->width_mm_edid_error = _E(CRTC_INFO_WIDTH_MM_EDID); /* TODO */
  e |= this->height_mm_edid_error = _E(CRTC_INFO_HEIGHT_MM_EDID); /* TODO */
  e |= this->gamma_error = _E(CRTC_INFO_GAMMA); /* TODO */
  e |= require_connector ? read_connector_data(this, connector, fields) : 0;
  e |= this->subpixel_order_error = _E(CRTC_INFO_SUBPIXEL_ORDER); /* TODO */
  e |= (fields & CRTC_INFO_GAMMA_SIZE) ? get_gamma_ramp_size(this, crtc) : 0;
  this->gamma_depth = 16;
  e |= this->gamma_support_error = _E(CRTC_INFO_GAMMA_SUPPORT);
  
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
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_linux_drm_crtc_get_gamma_ramps(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps_t* restrict ramps)
{
  libgamma_drm_card_data_t* card = this->partition->data;
  int r;
#ifdef DEBUG
  if ((ramps->red_size != ramps->green_size) ||
      (ramps->red_size != ramps->blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  r = drmModeCrtcGetGamma(card->fd, (uint32_t)(this->crtc), (uint32_t)(ramps->red_size),
			  ramps->red, ramps->green, ramps->blue);
  return r ? LIBGAMMA_GAMMA_RAMP_READ_FAILED : 0;
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @param   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_linux_drm_crtc_set_gamma_ramps(libgamma_crtc_state_t* restrict this,
					    libgamma_gamma_ramps_t ramps)
{
  libgamma_drm_card_data_t* card = this->partition->data;
  int r;
#ifdef DEBUG
  if ((ramps.red_size != ramps.green_size) ||
      (ramps.red_size != ramps.blue_size))
    return LIBGAMMA_MIXED_GAMMA_RAMP_SIZE;
#endif
  r = drmModeCrtcSetGamma(card->fd, (uint32_t)(size_t)(this->data),
			  (uint32_t)(ramps.red_size), ramps.red, ramps.green, ramps.blue);
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
	 * monitor but I imagine either of these is what would happen. */
	return LIBGAMMA_GRAPHICS_CARD_REMOVED;

      default:
	return LIBGAMMA_ERRNO_SET;
      }
  return 0;
}

