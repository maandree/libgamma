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
#include "libgamma-facade.h"

#include "libgamma-error.h"
#include "libgamma-method.h"
#ifdef HAVE_GAMMA_METHOD_DUMMY
# include "gamma-dummy.h"
# ifndef HAVE_GAMMA_METHODS
#  define HAVE_GAMMA_METHODS
# endif
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
# include "gamma-randr.h"
# ifndef HAVE_GAMMA_METHODS
#  define HAVE_GAMMA_METHODS
# endif
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
# include "gamma-vidmode.h"
# ifndef HAVE_GAMMA_METHODS
#  define HAVE_GAMMA_METHODS
# endif
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
# include "gamma-drm.h"
# ifndef HAVE_GAMMA_METHODS
#  define HAVE_GAMMA_METHODS
# endif
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
# include "gamma-w32gdi.h"
# ifndef HAVE_GAMMA_METHODS
#  define HAVE_GAMMA_METHODS
# endif
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
# include "gamma-quartz-cg.h"
# ifndef HAVE_GAMMA_METHODS
#  define HAVE_GAMMA_METHODS
# endif
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#ifndef HAVE_GAMMA_METHODS
# define HAVE_NO_GAMMA_METHODS
#endif


#ifdef HAVE_NO_GAMMA_METHODS
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsuggest-attribute=const"
#endif


/**
 * Return the capabilities of an adjustment method
 * 
 * @param  this    The data structure to fill with the method's capabilities
 * @param  method  The adjustment method (display server and protocol)
 */
void libgamma_method_capabilities(libgamma_method_capabilities_t* restrict this, int method)
{
  memset(this, 0, sizeof(libgamma_method_capabilities_t));
  
  switch (method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      libgamma_dummy_method_capabilities(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      libgamma_randr_method_capabilities(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      libgamma_vidmode_method_capabilities(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      libgamma_drm_method_capabilities(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      libgamma_w32gdi_method_capabilities(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      libgamma_quartz_cg_method_capabilities(this);
      break;
#endif
      
    default:
      /* Method does not exists/excluded at compile-time.
         We will assume that this is not done... */
      break;
    }
}


/**
 * Return the capabilities of an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The default site, `NULL` if it cannot be determined or
 *                  if multiple sites are not supported by the adjustment
 *                  method. This value should not be free:d.
 */
char* libgamma_method_default_site(int method)
{
  const char* var = libgamma_method_default_site_variable(method);
  char* env;
  
  if (var == NULL)
    return NULL;
  
  env = getenv(var);
  if ((env == NULL) || (*env == '\0'))
    return NULL;
  
  return env;
}


/**
 * Return the capabilities of an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The environ variables that is used to determine the
 *                  default site. `NULL` if there is none, that is, if
 *                  the method does not support multiple sites.
 *                  This value should not be free:d.
 */
const char* libgamma_method_default_site_variable(int method)
{
  switch (method)
    {
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return "DISPLAY";
#endif
      
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return "DISPLAY";
#endif
      
    default:
      return NULL;
    }
}



/**
 * Initialise an allocated site state
 * 
 * @param   this    The site state to initialise
 * @param   method  The adjustment method (display server and protocol)
 * @param   site    The site identifier, unless it is `NULL` it must a
 *                  `free`:able. One the state is destroyed the library
 *                  will attempt to free it. There you should not free
 *                  it yourself, and it must not be a string constant
 *                  or allocate on the stack. Note however that it will
 *                  not be free:d if this function fails.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int libgamma_site_initialise(libgamma_site_state_t* restrict this,
			     int method, char* restrict site)
{
  this->method = method;
  this->site = site;
  
  switch (method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_site_initialise(this, site);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_site_initialise(this, site);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_site_initialise(this, site);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_site_initialise(this, site);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_site_initialise(this, site);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_site_initialise(this, site);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_site_destroy(libgamma_site_state_t* restrict this)
{
  switch (this->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      libgamma_dummy_site_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      libgamma_randr_site_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      libgamma_vidmode_site_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      libgamma_drm_site_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      libgamma_w32gdi_site_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      libgamma_quartz_cg_site_destroy(this);
      break;
#endif
      
    default:
      /* Method does not exists/excluded at compile-time.
         We will assume that this is not done... */
      break;
    }
  free(this->site);
}


/**
 * Release all resources held by a site state
 * and free the site state pointer
 * 
 * @param  this  The site state
 */
void libgamma_site_free(libgamma_site_state_t* restrict this)
{
  libgamma_site_destroy(this);
  free(this);
}


/**
 * Restore the gamma ramps all CRTCS with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_site_restore(libgamma_site_state_t* restrict this)
{
  switch (this->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_site_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_site_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_site_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_site_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_site_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_site_restore(this);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
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
int libgamma_partition_initialise(libgamma_partition_state_t* restrict this,
				  libgamma_site_state_t* restrict site, size_t partition)
{
  this->site = site;
  this->partition = partition;
  
  switch (site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_partition_initialise(this, site, partition);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_partition_initialise(this, site, partition);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_partition_initialise(this, site, partition);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_partition_initialise(this, site, partition);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_partition_initialise(this, site, partition);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_partition_initialise(this, site, partition);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_partition_destroy(libgamma_partition_state_t* restrict this)
{
  switch (this->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      libgamma_dummy_partition_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      libgamma_randr_partition_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      libgamma_vidmode_partition_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      libgamma_drm_partition_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      libgamma_w32gdi_partition_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      libgamma_quartz_cg_partition_destroy(this);
      break;
#endif
      
    default:
      /* Method does not exists/excluded at compile-time.
         We will assume that this is not done... */
      break;
    }
}


/**
 * Release all resources held by a partition state
 * and free the partition state pointer
 * 
 * @param  this  The partition state
 */
void libgamma_partition_free(libgamma_partition_state_t* restrict this)
{
  libgamma_partition_destroy(this);
  free(this);
}


/**
 * Restore the gamma ramps all CRTCS with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_partition_restore(libgamma_partition_state_t* restrict this)
{
  switch (this->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_partition_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_partition_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_partition_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_partition_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_partition_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_partition_restore(this);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
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
int libgamma_crtc_initialise(libgamma_crtc_state_t* restrict this,
			     libgamma_partition_state_t* restrict partition, size_t crtc)
{
  this->partition = partition;
  this->crtc = crtc;
  
  switch (partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_initialise(this, partition, crtc);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_initialise(this, partition, crtc);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_initialise(this, partition, crtc);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_initialise(this, partition, crtc);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_initialise(this, partition, crtc);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_initialise(this, partition, crtc);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      libgamma_dummy_crtc_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      libgamma_randr_crtc_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      libgamma_vidmode_crtc_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      libgamma_drm_crtc_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      libgamma_w32gdi_crtc_destroy(this);
      break;
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      libgamma_quartz_cg_crtc_destroy(this);
      break;
#endif
      
    default:
      /* Method does not exists/excluded at compile-time.
         We will assume that this is not done... */
      break;
    }
}


/**
 * Release all resources held by a CRTC state
 * and free the CRTC state pointer
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_free(libgamma_crtc_state_t* restrict this)
{
  libgamma_crtc_destroy(this);
  free(this);
}


/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_crtc_restore(libgamma_crtc_state_t* restrict this)
{
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_restore(this);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_restore(this);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}



/**
 * Read information about a CRTC
 * 
 * @param   this    Instance of a data structure to fill with the information about the CRTC
 * @param   crtc    The state of the CRTC whose information should be read
 * @param   fields  OR:ed identifiers for the information about the CRTC that should be read
 * @return          Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_get_crtc_information(libgamma_crtc_information_t* restrict this,
				  libgamma_crtc_state_t* restrict crtc, int32_t fields)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) this;
  (void) fields;
#endif
  
  switch (crtc->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_get_crtc_information(this, crtc, fields);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_get_crtc_information(this, crtc, fields);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_get_crtc_information(this, crtc, fields);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_get_crtc_information(this, crtc, fields);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_get_crtc_information(this, crtc, fields);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_get_crtc_information(this, crtc, fields);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Release all resources in an information data structure for a CRTC
 * 
 * @param  this  The CRTC information
 */
void libgamma_crtc_information_destroy(libgamma_crtc_information_t* restrict this)
{
  free(this->edid);
  free(this->connector_name);
}


/**
 * Release all resources in an information data structure for a CRTC
 * and free the data structure pointer
 * 
 * @param  this  The CRTC information
 */
void libgamma_crtc_information_free(libgamma_crtc_information_t* restrict this)
{
  libgamma_crtc_information_destroy(this);
  free(this);
}


/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @retrun          The EDID in lowercase hexadecimal representation,
 *                  `NULL` on allocation error, `errno` will be set accordingly
 */
char* libgamma_behex_edid_lowercase(const unsigned char* restrict edid, size_t length)
{
  char* out = malloc((length * 2 + 1) * sizeof(char));
  size_t i;
  
  if (out == NULL)
    return NULL;
  
  for (i = 0; i < length; i++)
    {
      out[i * 2 + 0] = "0123456789abcdef"[(edid[i] >> 4) & 15];
      out[i * 2 + 1] = "0123456789abcdef"[(edid[i] >> 0) & 15];
    }
  out[length * 2] = '\0';
  
  return out;
}


/**
 * Convert a raw representation of an EDID to an uppercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @retrun          The EDID in uppercase hexadecimal representation,
 *                  NULL` on allocation error, `errno` will be set accordingly
 */
char* libgamma_behex_edid_uppercase(const unsigned char* restrict edid, size_t length)
{
  char* out = malloc((length * 2 + 1) * sizeof(char));
  size_t i;
  
  if (out == NULL)
    return NULL;
  
  for (i = 0; i < length; i++)
    {
      out[i * 2 + 0] = "0123456789ABCDEF"[(edid[i] >> 4) & 15];
      out[i * 2 + 1] = "0123456789ABCDEF"[(edid[i] >> 0) & 15];
    }
  out[length * 2] = '\0';
  
  return out;
}


/**
 * Convert an hexadecimal representation of an EDID to a raw representation
 * 
 * @param   edid  The EDID in hexadecimal representation
 * @retrun        The EDID in raw representation, it will be half the length
 *                of `edid` (the input value). `NULL` on allocation error or
 *                if the EDID is malformated, `errno` will be set accordingly.
 */
unsigned char* libgamma_unhex_edid(const char* restrict edid)
{
#define not_range(lower, V, upper)  ((V < lower) || (upper < V))
#define is_not_hex(V)  (not_range('0', V, '9') && not_range('a', V, 'f') && not_range('A', V, 'F'))
  
  unsigned char* out = NULL;
  size_t n = strlen(edid);
  size_t i;
  
  if ((n & 1))
    return errno = EINVAL, NULL;
  
  out = malloc(n / 2 * sizeof(unsigned char));
  if (out == NULL)
    return NULL;
  
  for (i = 0; i < n; i++)
    {
      char a = edid[i * 2 + 0];
      char b = edid[i * 2 + 1];
      
      if (is_not_hex(a) || is_not_hex(b))
	{
	  free(out);
	  return errno = EINVAL, NULL;
	}
      
      a = (char)((a & 15) + (a > '9' ? 9 : 0));
      b = (char)((b & 15) + (b > '9' ? 9 : 0));
      
      out[i] = (unsigned char)((a << 4) | b);
    }
  
  return out;
  
#undef is_hex
#undef not_range
}



/**
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_t* restrict ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_get_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_get_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_get_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_get_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_get_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_get_gamma_ramps(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps(libgamma_crtc_state_t* restrict this,
				  libgamma_gamma_ramps_t ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_set_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_set_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_set_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_set_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_set_gamma_ramps(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_set_gamma_ramps(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}



/**
 * Get current the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps32(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps32_t* restrict ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_get_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_get_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_get_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_get_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_get_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_get_gamma_ramps32(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps32_t ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_set_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_set_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_set_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_set_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_set_gamma_ramps32(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_set_gamma_ramps32(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}



/**
 * Get current the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps64(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps64_t* restrict ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_get_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_get_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_get_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_get_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_get_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_get_gamma_ramps64(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps64_t ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_set_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_set_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_set_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_set_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_set_gamma_ramps64(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_set_gamma_ramps64(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}



/**
 * Get current the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_rampsf(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsf_t* restrict ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_get_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_get_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_get_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_get_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_get_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_get_gamma_rampsf(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Set the gamma ramps for a CRTC, `float` version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_rampsf(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsf_t ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_set_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_set_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_set_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_set_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_set_gamma_rampsf(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_set_gamma_rampsf(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}



/**
 * Get current the gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_rampsd(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsd_t* restrict ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_get_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_get_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_get_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_get_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_get_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_get_gamma_rampsd(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}


/**
 * Set the gamma ramps for a CRTC, `double` version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_rampsd(libgamma_crtc_state_t* restrict this,
				   libgamma_gamma_rampsd_t ramps)
{
#ifdef HAVE_NO_GAMMA_METHODS
  (void) ramps;
#endif
  
  switch (this->partition->site->method)
    {
#ifdef HAVE_GAMMA_METHOD_DUMMY
    case GAMMA_METHOD_DUMMY:
      return libgamma_dummy_crtc_set_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_RANDR
    case GAMMA_METHOD_X_RANDR:
      return libgamma_randr_crtc_set_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_X_VIDMODE
    case GAMMA_METHOD_X_VIDMODE:
      return libgamma_vidmode_crtc_set_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_LINUX_DRM
    case GAMMA_METHOD_LINUX_DRM:
      return libgamma_drm_crtc_set_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_W32_GDI
    case GAMMA_METHOD_W32_GDI:
      return libgamma_w32gdi_crtc_set_gamma_rampsd(this, ramps);
#endif
#ifdef HAVE_GAMMA_METHOD_QUARTZ_CORE_GRAPHICS
    case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
      return libgamma_quartz_cg_crtc_set_gamma_rampsd(this, ramps);
#endif
      
    default:
      return LIBGAMMA_NO_SUCH_ADJUSTMENT_METHOD;
    }
}



/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps_f(libgamma_crtc_state_t* restrict this,
				    libgamma_gamma_ramps_fun* red_function,
				    libgamma_gamma_ramps_fun* green_function,
				    libgamma_gamma_ramps_fun* blue_function)
{
  libgamma_crtc_information_t info;
  libgamma_gamma_ramps_t ramps;
  size_t i, n;
  int e;
  
  if (libgamma_get_crtc_information(&info, this, CRTC_INFO_GAMMA_SIZE))
    {
      e = info.gamma_size_error;
      if (e < 0)
	return e;
      return errno = e, LIBGAMMA_ERRNO_SET;
    }
  
  n  = ramps.  red_size = info.  red_gamma_size;
  n += ramps.green_size = info.green_gamma_size;
  n += ramps. blue_size = info. blue_gamma_size;
  
  ramps.  red = malloc(n * sizeof(uint16_t));
  ramps.green = ramps.  red + ramps.  red_size;
  ramps. blue = ramps.green + ramps.green_size;
  if (ramps.red == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  for (i = 0, n = ramps.red_size; i < n; i++)
    ramps.red[i] = red_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.green_size; i < n; i++)
    ramps.green[i] = green_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.blue_size; i < n; i++)
    ramps.blue[i] = blue_function((float)i / (float)(n - 1));
  
  e = libgamma_crtc_set_gamma_ramps(this, ramps);
  free(ramps.red);
  return e;
}


/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps32_f(libgamma_crtc_state_t* restrict this,
				      libgamma_gamma_ramps32_fun* red_function,
				      libgamma_gamma_ramps32_fun* green_function,
				      libgamma_gamma_ramps32_fun* blue_function)
{
  libgamma_crtc_information_t info;
  libgamma_gamma_ramps32_t ramps;
  size_t i, n;
  int e;
  
  if (libgamma_get_crtc_information(&info, this, CRTC_INFO_GAMMA_SIZE))
    {
      e = info.gamma_size_error;
      if (e < 0)
	return e;
      return errno = e, LIBGAMMA_ERRNO_SET;
    }
  
  n  = ramps.  red_size = info.  red_gamma_size;
  n += ramps.green_size = info.green_gamma_size;
  n += ramps. blue_size = info. blue_gamma_size;
  
  ramps.  red = malloc(n * sizeof(uint32_t));
  ramps.green = ramps.  red + ramps.  red_size;
  ramps. blue = ramps.green + ramps.green_size;
  if (ramps.red == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  for (i = 0, n = ramps.red_size; i < n; i++)
    ramps.red[i] = red_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.green_size; i < n; i++)
    ramps.green[i] = green_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.blue_size; i < n; i++)
    ramps.blue[i] = blue_function((float)i / (float)(n - 1));
  
  e = libgamma_crtc_set_gamma_ramps32(this, ramps);
  free(ramps.red);
  return e;
}


/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps64_f(libgamma_crtc_state_t* restrict this,
				      libgamma_gamma_ramps64_fun* red_function,
				      libgamma_gamma_ramps64_fun* green_function,
				      libgamma_gamma_ramps64_fun* blue_function)
{
  libgamma_crtc_information_t info;
  libgamma_gamma_ramps64_t ramps;
  size_t i, n;
  int e;
  
  if (libgamma_get_crtc_information(&info, this, CRTC_INFO_GAMMA_SIZE))
    {
      e = info.gamma_size_error;
      if (e < 0)
	return e;
      return errno = e, LIBGAMMA_ERRNO_SET;
    }
  
  n  = ramps.  red_size = info.  red_gamma_size;
  n += ramps.green_size = info.green_gamma_size;
  n += ramps. blue_size = info. blue_gamma_size;
  
  ramps.  red = malloc(n * sizeof(uint64_t));
  ramps.green = ramps.  red + ramps.  red_size;
  ramps. blue = ramps.green + ramps.green_size;
  if (ramps.red == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  for (i = 0, n = ramps.red_size; i < n; i++)
    ramps.red[i] = red_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.green_size; i < n; i++)
    ramps.green[i] = green_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.blue_size; i < n; i++)
    ramps.blue[i] = blue_function((float)i / (float)(n - 1));
  
  e = libgamma_crtc_set_gamma_ramps64(this, ramps);
  free(ramps.red);
  return e;
}


/**
 * Set the gamma ramps for a CRTC, `float` function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_rampsf_f(libgamma_crtc_state_t* restrict this,
				     libgamma_gamma_rampsf_fun* red_function,
				     libgamma_gamma_rampsf_fun* green_function,
				     libgamma_gamma_rampsf_fun* blue_function)
{
  libgamma_crtc_information_t info;
  libgamma_gamma_rampsf_t ramps;
  size_t i, n;
  int e;
  
  if (libgamma_get_crtc_information(&info, this, CRTC_INFO_GAMMA_SIZE))
    {
      e = info.gamma_size_error;
      if (e < 0)
	return e;
      return errno = e, LIBGAMMA_ERRNO_SET;
    }
  
  n  = ramps.  red_size = info.  red_gamma_size;
  n += ramps.green_size = info.green_gamma_size;
  n += ramps. blue_size = info. blue_gamma_size;
  
  ramps.  red = malloc(n * sizeof(float));
  ramps.green = ramps.  red + ramps.  red_size;
  ramps. blue = ramps.green + ramps.green_size;
  if (ramps.red == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  for (i = 0, n = ramps.red_size; i < n; i++)
    ramps.red[i] = red_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.green_size; i < n; i++)
    ramps.green[i] = green_function((float)i / (float)(n - 1));
  
  for (i = 0, n = ramps.blue_size; i < n; i++)
    ramps.blue[i] = blue_function((float)i / (float)(n - 1));
  
  e = libgamma_crtc_set_gamma_rampsf(this, ramps);
  free(ramps.red);
  return e;
}


/**
 * Set the gamma ramps for a CRTC, `double` function version
 * 
 * Note that this will probably involve the library allocating temporary data
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_rampsd_f(libgamma_crtc_state_t* restrict this,
				     libgamma_gamma_rampsd_fun* red_function,
				     libgamma_gamma_rampsd_fun* green_function,
				     libgamma_gamma_rampsd_fun* blue_function)
{
  libgamma_crtc_information_t info;
  libgamma_gamma_rampsd_t ramps;
  size_t i, n;
  int e;
  
  if (libgamma_get_crtc_information(&info, this, CRTC_INFO_GAMMA_SIZE))
    {
      e = info.gamma_size_error;
      if (e < 0)
	return e;
      return errno = e, LIBGAMMA_ERRNO_SET;
    }
  
  n  = ramps.  red_size = info.  red_gamma_size;
  n += ramps.green_size = info.green_gamma_size;
  n += ramps. blue_size = info. blue_gamma_size;
  
  ramps.  red = malloc(n * sizeof(double));
  ramps.green = ramps.  red + ramps.  red_size;
  ramps. blue = ramps.green + ramps.green_size;
  if (ramps.red == NULL)
    return LIBGAMMA_ERRNO_SET;
  
  for (i = 0, n = ramps.red_size; i < n; i++)
    ramps.red[i] = red_function((double)i / (double)(n - 1));
  
  for (i = 0, n = ramps.green_size; i < n; i++)
    ramps.green[i] = green_function((double)i / (double)(n - 1));
  
  for (i = 0, n = ramps.blue_size; i < n; i++)
    ramps.blue[i] = blue_function((double)i / (double)(n - 1));
  
  e = libgamma_crtc_set_gamma_rampsd(this, ramps);
  free(ramps.red);
  return e;
}



#ifdef HAVE_NO_GAMMA_METHODS
# pragma GCC diagnostic pop
#endif

