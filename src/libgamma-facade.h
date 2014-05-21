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
#ifndef LIBGAMMA_FACADE_H
#define LIBGAMMA_FACADE_H


#include "libgamma-method.h"

#include <stddef.h>
#include <stdint.h>


/**
 * Return the capabilities of an adjustment method
 * 
 * @param  this    The data structure to fill with the method's capabilities
 * @param  method  The adjustment method (display server and protocol)
 */
void libgamma_method_capabilities(libgamma_method_capabilities_t* this, int method);

/**
 * Return the capabilities of an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The default site, `NULL` if it cannot be determined or
 *                  if multiple sites are not supported by the adjustment
 *                  method. This value should not be free:d.
 */
char* libgamma_method_default_site(int method);

/**
 * Return the capabilities of an adjustment method
 * 
 * @param   method  The adjustment method (display server and protocol)
 * @return          The environ variables that is used to determine the
 *                  default site. `NULL` if there is none, that is, if
 *                  the method does not support multiple sites.
 *                  This value should not be free:d.
 */
const char* libgamma_method_default_site_variable(int method);


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
int libgamma_site_initialise(libgamma_site_state_t* this,
			     int method, char* site);

/**
 * Release all resources held by a site state
 * 
 * @param  this  The site state
 */
void libgamma_site_destroy(libgamma_site_state_t* this);

/**
 * Release all resources held by a site state
 * and free the site state pointer
 * 
 * @param  this  The site state
 */
void libgamma_site_free(libgamma_site_state_t* this);

/**
 * Restore the gamma ramps all CRTCS with a site to the system settings
 * 
 * @param   this  The site state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_site_restore(libgamma_site_state_t* this);


/**
 * Initialise an allocated partition state
 * 
 * @param   this       The partition state to initialise
 * @param   site       The site state for the site that the partition belongs to
 * @param   partition  The the index of the partition within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_partition_initialise(libgamma_partition_state_t* this,
				  libgamma_site_state_t* site, size_t partition);

/**
 * Release all resources held by a partition state
 * 
 * @param  this  The partition state
 */
void libgamma_partition_destroy(libgamma_partition_state_t* this);

/**
 * Release all resources held by a partition state
 * and free the partition state pointer
 * 
 * @param  this  The partition state
 */
void libgamma_partition_free(libgamma_partition_state_t* this);

/**
 * Restore the gamma ramps all CRTCS with a partition to the system settings
 * 
 * @param   this  The partition state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_partition_restore(libgamma_site_state_t* this);


/**
 * Initialise an allocated CRTC state
 * 
 * @param   this       The CRTC state to initialise
 * @param   partition  The partition state for the partition that the CRTC belongs to
 * @param   crtc       The the index of the CRTC within the site
 * @return             Zero on success, otherwise (negative) the value of an
 *                     error identifier provided by this library
 */
int libgamma_crtc_initialise(libgamma_crtc_state_t* this,
			     libgamma_partition_state_t* partition, size_t crtc);

/**
 * Release all resources held by a CRTC state
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_destroy(libgamma_crtc_state_t* this);

/**
 * Release all resources held by a CRTC state
 * and free the CRTC state pointer
 * 
 * @param  this  The CRTC state
 */
void libgamma_crtc_free(libgamma_crtc_state_t* this);

/**
 * Restore the gamma ramps for a CRTC to the system settings for that CRTC
 * 
 * @param   this  The CRTC state
 * @return        Zero on success, otherwise (negative) the value of an
 *                error identifier provided by this library
 */
int libgamma_crtc_restore(libgamma_site_state_t* this);


/**
 * Read information about a CRTC
 * 
 * @param   this   Instance of a data structure to fill with the information about the CRTC
 * @param   crtc   The state of the CRTC whose information should be read
 * @param   field  OR:ed identifiers for the information about the CRTC that should be read
 * @return         Zero on success, -1 on error. On error refer to the error reports in `this`.
 */
int libgamma_get_crtc_information(libgamma_crtc_information_t* this,
				  libgamma_crtc_state_t* crtc, int32_t field);

/**
 * Release all resources in an information data structure for a CRTC
 * 
 * @param  this  The CRTC information
 */
int libgamma_crtc_information_destroy(libgamma_crtc_information_t* this);

/**
 * Release all resources in an information data structure for a CRTC
 * and free the data structure pointer
 * 
 * @param  this  The CRTC information
 */
int libgamma_crtc_information_free(libgamma_crtc_information_t* this);

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid:const unsigned char*  The EDID in raw representation
 * @param   length:size_t              The length of `edid`
 * @return  :char*                     The EDID in lowercase hexadecimal representation,
 *                                     `NULL` on allocation error, `errno` will be set accordingly
 */
#define libgamma_behex_edid(edid, length)  \
  libgamma_behex_edid_lowercase(edid, length)

/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @retrun          The EDID in lowercase hexadecimal representation,
 *                  `NULL` on allocation error, `errno` will be set accordingly
 */
char* libgamma_behex_edid_lowercase(const unsigned char* edid, size_t length);

/**
 * Convert a raw representation of an EDID to an uppercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @retrun          The EDID in uppercase hexadecimal representation,
 *                  NULL` on allocation error, `errno` will be set accordingly
 */
char* libgamma_behex_edid_uppercase(const unsigned char* edid, size_t length);

/**
 * Convert an hexadecimal representation of an EDID to a raw representation
 * 
 * @param   edid  The EDID in hexadecimal representation
 * @retrun        The EDID in raw representation, it will be twice the length
 *                of `edid` (the input value). `NULL` on allocation error,
 *                `errno` will be set accordingly.
 */
unsigned char* libgamma_unhex_edid(const char* edid);


/**
 * Set the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps(libgamma_crtc_state_t* this, libgamma_gamma_ramps_t ramps);

/**
 * Get current the gamma ramps for a CRTC, 16-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps(libgamma_crtc_state_t* this, libgamma_gamma_ramps_t* ramps);


/**
 * Set the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps32(libgamma_crtc_state_t* this, libgamma_gamma_ramps32_t ramps);

/**
 * Get current the gamma ramps for a CRTC, 32-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps32(libgamma_crtc_state_t* this, libgamma_gamma_ramps32_t* ramps);


/**
 * Set the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to apply
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_set_gamma_ramps64(libgamma_crtc_state_t* this, libgamma_gamma_ramps64_t ramps);

/**
 * Get current the gamma ramps for a CRTC, 64-bit gamma-depth version
 * 
 * @param   this   The CRTC state
 * @parma   ramps  The gamma ramps to fill with the current values
 * @return         Zero on success, otherwise (negative) the value of an
 *                 error identifier provided by this library
 */
int libgamma_crtc_get_gamma_ramps64(libgamma_crtc_state_t* this, libgamma_gamma_ramps64_t* ramps);


#endif

