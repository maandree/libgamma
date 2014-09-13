/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014  Mattias Andrée (maandree@member.fsf.org)
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
#ifndef LIBGAMMA_TEST_USER_H
#define LIBGAMMA_TEST_USER_H


#include "methods.h"

#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>



/**
 * Let the user select adjustment method, site, partition and CRTC.
 * 
 * @param   site_state  Output slot for the site.
 * @param   part_state  Output slot for the partition.
 * @param   crtc_state  Output slot for the CRTC.
 * @return              Zero on and only on success.
 */
int select_monitor(libgamma_site_state_t* restrict site_state,
		   libgamma_partition_state_t* restrict part_state,
		   libgamma_crtc_state_t* restrict crtc_state);


#endif

