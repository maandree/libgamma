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
#ifndef LIBGAMMA_TEST_METHODS_H
#define LIBGAMMA_TEST_METHODS_H


#ifndef __GNUC__
# define __attribute__()
#endif

/**
 * Get the name representation of an
 * adjustment method by its identifier.
 * 
 * @param   method  The identifier of the adjustment method.
 * @return          The name of the adjustment method, should not be `free`:d.
 */
const char* method_name(int method) __attribute__((const));

/**
 * Print a list of adjustment methods.
 * 
 * @param  description  Precursory text for the list.
 * @param  methods      An array allocated to fit all adjustment methods.
 * @param  operation    See the `operation` parameter for `libgamma_list_methods`.
 */
void list_methods(const char* description, int* methods, int operation);

/**
 * Print all lists, of adjustments methods, that
 * are made available by `libgamma_list_methods`.
 */
void list_methods_lists(void);

/**
 * Test the availability (determined
 * at compile-time) of all adjustment
 * methods and one that does not exist.
 */
void method_availability(void);

/**
 * List the default site and the environment
 * variable, if any, that determines the
 * default site, for all availiable adjustment
 * methods.
 */
void list_default_sites(void);

/**
 * Print the capabilities of all availiable
 * adjustment methods.
 */
void method_capabilities(void);


#endif

