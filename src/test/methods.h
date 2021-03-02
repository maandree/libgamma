/* See LICENSE file for copyright and license details. */
#ifndef LIBGAMMA_TEST_METHODS_H
#define LIBGAMMA_TEST_METHODS_H


#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>


#ifndef __GCC__
# define __attribute__(x)
#endif


/**
 * Get the name representation of an
 * adjustment method by its identifier
 * 
 * @param   method  The identifier of the adjustment method
 * @return          The name of the adjustment method, should not be `free`:d
 */
const char *method_name(int method) __attribute__((const));

/**
 * Print a list of adjustment methods
 * 
 * @param  description  Precursory text for the list
 * @param  methods      An array allocated to fit all adjustment methods
 * @param  operation    See the `operation` parameter for `libgamma_list_methods`
 */
void list_methods(const char *description, int *methods, int operation);

/**
 * Print all lists, of adjustments methods, that
 * are made available by `libgamma_list_methods`
 */
void list_methods_lists(void);

/**
 * Test the availability (determined
 * at compile-time) of all adjustment
 * methods and one that does not exist
 */
void method_availability(void);

/**
 * List the default site and the environment
 * variable, if any, that determines the
 * default site, for all availiable adjustment
 * methods
 */
void list_default_sites(void);

/**
 * Print the capabilities of all availiable
 * adjustment methods
 */
void method_capabilities(void);


#endif
