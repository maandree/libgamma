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
#ifndef LIBGAMMA_EDID_H
#define LIBGAMMA_EDID_H

#include "libgamma-method.h"

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
int libgamma_parse_edid(libgamma_crtc_information_t* restrict out, int32_t fields);


#endif

