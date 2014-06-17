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
#include "methods.h"

#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>


/**
 * Get the name representation of an
 * adjustment method by its identifier.
 * 
 * @param   method  The identifier of the adjustment method.
 * @return          The name of the adjustment method, should not be `free`:d.
 */
const char* method_name(int method)
{
  switch (method)
    {
    case LIBGAMMA_METHOD_DUMMY:                 return "dummy";
    case LIBGAMMA_METHOD_X_RANDR:               return "RandR X extension";
    case LIBGAMMA_METHOD_X_VIDMODE:             return "VidMode X extension";
    case LIBGAMMA_METHOD_LINUX_DRM:             return "Linux DRM";
    case LIBGAMMA_METHOD_W32_GDI:               return "Windows GDI";
    case LIBGAMMA_METHOD_QUARTZ_CORE_GRAPHICS:  return "Quartz using CoreGraphics";
    default:
      return "(unknown)";
    }
}


/**
 * Print a list of adjustment methods.
 * 
 * @param  description  Precursory text for the list.
 * @param  methods      An array allocated to fit all adjustment methods.
 * @param  operation    See the `operation` parameter for `libgamma_list_methods`.
 */
void list_methods(const char* description, int* methods, int operation)
{
  /* Get adjustment method list. */
  size_t i, n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, operation);
  
  /* Print adjustment method list. */
  printf("%s:\n", description);
  for (i = 0; i < n; i++)
    printf("  %s\n", method_name(methods[i]));
  printf("\n");
}


/**
 * Print all lists, of adjustments methods, that
 * are made available by `libgamma_list_methods`.
 */
void list_methods_lists(void)
{
  /* Allocate a list for adjustment methods that is
     large enough if the program is up to date with
     the library. */
  int* methods = malloc(LIBGAMMA_METHOD_COUNT * sizeof(int));
  /* Get a list of all adjustment methods. */
  size_t n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, 4);
  
  /* If we got more adjustment methods than we expected
     the program needs to be recompiled, and we need to
     reallocate the list so it is large enough. */
  if (n > LIBGAMMA_METHOD_COUNT)
    {
      printf("Warning: you should to recompile the program, libgamma has been updated.\n");
      methods = realloc(methods, n * sizeof(int));
    }
  
  /* Print adjustment method lists. */
  list_methods("Available adjustment methods",               methods, 4);
  list_methods("Available real adjustment methods",          methods, 3);
  list_methods("Available real non-fake adjustment methods", methods, 2);
  list_methods("Recommended adjustment methods",             methods, 1);
  list_methods("Recommended non-fake adjustment methods",    methods, 0);
  
  free(methods);
}


/**
 * Test the availability (determined
 * at compile-time) of all adjustment
 * methods and one that does not exist.
 */
void method_availability(void)
{
  int method;
  /* The availability of an adjustmen method
     whose identifier is invalid. It should
     say it is not available. */
  printf("Testing the availability of a non-existing adjustment method: ");
  printf("%s\n", libgamma_is_method_available(-1) ? "available" : "not available");
  
  /* Test the availability  of the adjustment
     methods that does exist. */
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    {
      printf("Testing the availability of %s: ", method_name(method));
      printf("%s\n", libgamma_is_method_available(method) ? "available" : "not available");
    }
  printf("\n");
}


/**
 * List the default site and the environment
 * variable, if any, that determines the
 * default site, for all availiable adjustment
 * methods.
 */
void list_default_sites(void)
{
  int method;
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    /* Include only available adjustment methods.
       If an adjustment method is not available
       it should print "(null)", but as that can
       be misguiding we exclude those methods. */
    if (libgamma_is_method_available(method))
      {
	printf("Default site for %s:\n", method_name(method));
	printf("  Variable: %s\n", libgamma_method_default_site_variable(method));
	printf("  Site name: %s\n", libgamma_method_default_site(method));
	printf("\n");
      }
}


/**
 * Print the capabilities of all availiable
 * adjustment methods.
 */
void method_capabilities(void)
{
  libgamma_method_capabilities_t caps;
  int method;
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    if (libgamma_is_method_available(method))
      {
	/* Print adjustment method name and get the
	   adjustment method's capabilities. */
	printf("Capabilities of %s:\n", method_name(method));
	libgamma_method_capabilities(&caps, method);
	
	/* Print capabilities. The CRTC information
	   capabilities is printed hexadecimal. See
	   the `LIBGAMMA_CRTC_INFO_*` definitions in
	   `src/lib/libgamma-method.h` for what each
	   bit represents. */
	printf("  %s: %X\n", "CRTC information",      caps.crtc_information);
	printf("  %s: %s\n", "Default site known",    caps.default_site_known            ? "yes" : "no");
	printf("  %s: %s\n", "Multiple sites",        caps.multiple_sites                ? "yes" : "no");
	printf("  %s: %s\n", "Multiple partitions",   caps.multiple_partitions           ? "yes" : "no");
	printf("  %s: %s\n", "Multiple crtcs",        caps.multiple_crtcs                ? "yes" : "no");
	printf("  %s: %s\n", "Graphics cards",        caps.partitions_are_graphics_cards ? "yes" : "no");
	printf("  %s: %s\n", "Site restore",          caps.site_restore                  ? "yes" : "no");
	printf("  %s: %s\n", "Partition restore",     caps.partition_restore             ? "yes" : "no");
	printf("  %s: %s\n", "CRTC restore",          caps.crtc_restore                  ? "yes" : "no");
	printf("  %s: %s\n", "Identical gamma sizes", caps.identical_gamma_sizes         ? "yes" : "no");
	printf("  %s: %s\n", "Fixed gamma size",      caps.fixed_gamma_size              ? "yes" : "no");
	printf("  %s: %s\n", "Fixed gamma depth",     caps.fixed_gamma_depth             ? "yes" : "no");
	printf("  %s: %s\n", "Real method",           caps.real                          ? "yes" : "no");
	printf("  %s: %s\n", "Fake method",           caps.fake                          ? "yes" : "no");
	printf("\n");
      }
}

