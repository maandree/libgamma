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
#include "libgamma-error.h"


#include <stddef.h>
#include <sys/types.h>


/**
 * Group that the user needs to be a member of if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned
 */
gid_t libgamma_group_gid = 0;

/**
 * Group that the user needs to be a member of if `LIBGAMMA_DEVICE_REQUIRE_GROUP` is returned,
 * `NULL` if the name of the group `libgamma_group_gid` cannot be determined
 */
const char* libgamma_group_name = NULL;

