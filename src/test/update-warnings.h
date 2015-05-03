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
#ifndef LIBGAMMA_TEST_UPDATE_WARNINGS_H
#define LIBGAMMA_TEST_UPDATE_WARNINGS_H

#include <libgamma.h>


/*
 * This file testes whether the program is out of date
 * when it is compiled. This is done by checking for newer
 * values of constants that tells us how much there is of
 * different things for which we may want to create
 * descriptions or otherwise handle especially.
 * 
 * To get the current value of for example `LIBGAMMA_ERROR_MIN`
 * simply run the command
 * 
 *     cpp <<EOF | tail -n 1
 *     #include <libgamma.h>
 *     LIBGAMMA_ERROR_MIN
 *     EOF
 *
 * However, we only do this for if GCC is used to
 * compile the program because the #warning CPP
 * directive is a GCC extension. If you are not
 * using GNU you may want to find another way to
 * accomplish this.
 */

#ifdef __GCC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wcpp"
# if LIBGAMMA_ERROR_MIN < -47
#  warning New error codes have been added to libgamma.
# endif
# if LIBGAMMA_METHOD_COUNT > 7
#  warning New adjust methods has been added to libgamma
# endif
# if LIBGAMMA_CONNECTOR_TYPE_COUNT > 20
#  warning New connector types have been added to libgamma.
# endif
# if LIBGAMMA_SUBPIXEL_ORDER_COUNT > 6
#  warning New subpixel orders have been added to libgamma.
# endif
# if LIBGAMMA_CRTC_INFO_COUNT > 13
#  warning New CRTC information fields have been added to libgamma.
# endif
# pragma GCC diagnostic pop
#endif


#endif

