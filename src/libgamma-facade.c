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

#include "libgamma-method.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/**
 * Return the capabilities of an adjustment method
 * 
 * @param  this    The data structure to fill with the method's capabilities
 * @param  method  The adjustment method (display server and protocol)
 */
void libgamma_method_capabilities(libgamma_method_capabilities_t* restrict this, int method)
{
  /* TODO */
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
  /* TODO */
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
 *                  or allocate on the stack.
 * @return          Zero on success, otherwise (negative) the value of an
 *                  error identifier provided by this library
 */
int libgamma_site_initialise(libgamma_site_state_t* restrict this,
			     int method, char* restrict site)
{
  /* TODO */
}


/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_site_destroy(libgamma_site_state_t* restrict this)
{
  /* TODO */
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
  /* TODO */
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
  /* TODO */
}


/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_partition_destroy(libgamma_partition_state_t* restrict this)
{
  /* TODO */
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
int libgamma_partition_restore(libgamma_site_state_t* restrict this)
{
  /* TODO */
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
  /* TODO */
}


/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_destroy(libgamma_crtc_state_t* restrict this)
{
  /* TODO */
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
int libgamma_crtc_restore(libgamma_site_state_t* restrict this)
{
  /* TODO */
}



/**
 * Read information about a CRTC
 * 
 * @param   this   Instance of a data structure to fill with the information about the CRTC
 * @param   crtc   The state of the CRTC whose information should be read
 * @param   field  OR:ed identifiers for the information about the CRTC that should be read
 * @return         Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_get_crtc_information(libgamma_crtc_information_t* restrict this,
				  libgamma_crtc_state_t* restrict crtc, int32_t field)
{
  /* TODO */
}


/**
 * Release all resources in an information data structure for a CRTC
 * 
 * @param  this  The CRTC information
 */
int libgamma_crtc_information_destroy(libgamma_crtc_information_t* restrict this)
{
  /* TODO */
}


/**
 * Release all resources in an information data structure for a CRTC
 * and free the data structure pointer
 * 
 * @param  this  The CRTC information
 */
int libgamma_crtc_information_free(libgamma_crtc_information_t* restrict this)
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
  /* TODO */
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
  /* TODO */
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
  /* TODO */
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
  /* TODO */
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
  /* TODO */
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
  /* TODO */
}

