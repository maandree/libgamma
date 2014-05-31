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
#include "edid.h"

#include "libgamma-method.h"
#include "libgamma-error.h"

#include <stdint.h>


/**
 * Parse the EDID of a monitor.
 * 
 * @param   this    Instance of a data structure to fill with the information about the EDID.
 *                  It must contain the EDID and its length.
 * @param   feilds  OR:ed identifiers for the information about the EDID that should be parsed.
 *                  Fields that do not have to do with EDID are ignored.
 * @return          Non-zero on error.
 */
int libgamma_parse_edid(libgamma_crtc_information_t* restrict this, int32_t fields)
{
#define __test_version(edid, major, minor)  (((edid)[18] == major) && ((edid)[19] == minor))
  
  int error = 0;
  int checksum = 0;
  size_t i;
  
  if (this->edid_length != 128)
    error = LIBGAMMA_EDID_LENGTH_UNSUPPORTED;
  else if ((this->edid[0] != 0x00) || (this->edid[1] != 0xFF) || (this->edid[2] != 0xFF) || (this->edid[3] != 0xFF) ||
	   (this->edid[4] != 0xFF) || (this->edid[5] != 0xFF) || (this->edid[6] != 0xFF) || (this->edid[7] != 0x00))
    error = LIBGAMMA_EDID_WRONG_MAGIC_NUMBER;
  else if (__test_version(this->edid, 1, 3) == 0)
    error = LIBGAMMA_EDID_REVISION_UNSUPPORTED;
  
  this->width_mm_edid_error = this->height_mm_edid_error = this->gamma_error = error;
  
  this->width_mm_edid = (size_t)(this->edid[21]) * 10;
  this->height_mm_edid = (size_t)(this->edid[22]) * 10;
  
  if ((fields & LIBGAMMA_CRTC_INFO_GAMMA) && (error == 0))
    {
      if (this->edid[23] == 0xFF)
	this->gamma_error = LIBGAMMA_GAMMA_NOT_SPECIFIED;
      else
	this->gamma_red = this->gamma_green = this->gamma_blue = (float)((int)(this->edid[23]) + 100) / 100.f;
    }
  
  if (error == 0)
    for (i = 0; i < this->edid_length; i++)
      checksum += (int)(this->edid[i]);
  if ((checksum & 255))
    {
      error = LIBGAMMA_EDID_CHECKSUM_ERROR;
      this->edid_error = this->width_mm_edid_error = this->height_mm_edid_error = error;
      this->gamma_error = this->gamma_error == LIBGAMMA_GAMMA_NOT_SPECIFIED
	? LIBGAMMA_GAMMA_NOT_SPECIFIED_AND_EDID_CHECKSUM_ERROR : 0;
    }
  
  return error | this->gamma_error;
  
#undef __test_version
}

