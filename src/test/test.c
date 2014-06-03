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
#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#ifdef __GNUC__
# if LIBGAMMA_ERROR_MIN < -46
#  warning New error codes have been added to libgamma.
# endif
# if LIBGAMMA_METHOD_COUNT > 6
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
#endif


static const char* method_name(int method)
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


static void list_methods(const char* description, int* methods, int operation)
{
  size_t i, n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, operation);
  printf("%s:\n", description);
  for (i = 0; i < n; i++)
    printf("  %s\n", method_name(methods[i]));
  printf("\n");
}


static void list_methods_lists(void)
{
  int* methods = malloc(LIBGAMMA_METHOD_COUNT * sizeof(int));
  size_t n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, 4);
  
  if (n > LIBGAMMA_METHOD_COUNT)
    {
      printf("Warning: you should to recompile the program, libgamma has been updated.\n");
      methods = realloc(methods, n * sizeof(int));
    }
  
  list_methods("Available adjustment methods",               methods, 4);
  list_methods("Available real adjustment methods",          methods, 3);
  list_methods("Available real non-fake adjustment methods", methods, 2);
  list_methods("Recommended adjustment methods",             methods, 1);
  list_methods("Recommended non-fake adjustment methods",    methods, 0);
  
  free(methods);
}


static void method_availability(void)
{
  int method;
  printf("Testing the availability of an non-existing adjustment method: ");
  printf("%s\n", libgamma_is_method_available(9999) ? "available" : "not available");
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    {
      printf("Testing the availability of %s: ", method_name(method));
      printf("%s\n", libgamma_is_method_available(method) ? "available" : "not available");
    }
  printf("\n");
}


static void list_default_sites(void)
{
  int method;
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    if (libgamma_is_method_available(method))
      {
	printf("Default site for %s:\n", method_name(method));
	printf("  Variable: %s\n", libgamma_method_default_site_variable(method));
	printf("  Site name: %s\n", libgamma_method_default_site(method));
	printf("\n");
      }
}


static void method_capabilities(void)
{
  libgamma_method_capabilities_t caps;
  int method;
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    if (libgamma_is_method_available(method))
      {
	printf("Capabilities of %s:\n", method_name(method));
	libgamma_method_capabilities(&caps, method);
	
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


static void error_test(void)
{
  int i;
  printf("Testing error API using LIBGAMMA_STATE_UNKNOWN:\n");
  printf("  Expecting %i: %i\n", LIBGAMMA_STATE_UNKNOWN, libgamma_value_of_error("LIBGAMMA_STATE_UNKNOWN"));
  printf("  Expecting %s: %s\n", "LIBGAMMA_STATE_UNKNOWN", libgamma_name_of_error(LIBGAMMA_STATE_UNKNOWN));
  printf("\n");
  printf("Testing libgamma_perror:\n");
  libgamma_perror("  Expecting LIBGAMMA_STATE_UNKNOWN", LIBGAMMA_STATE_UNKNOWN);
  libgamma_perror("  Expecting an description for ENOMEM", ENOMEM);
  libgamma_perror("  Expecting an description for successfulness", 0);
  libgamma_perror("  Expecting an description for ENOMEM", (errno = ENOMEM, LIBGAMMA_ERRNO_SET));
  libgamma_group_gid = 10;
  libgamma_group_name = "test";
  libgamma_perror("  Expecting 'LIBGAMMA_DEVICE_REQUIRE_GROUP: test (10)'", LIBGAMMA_DEVICE_REQUIRE_GROUP);
  libgamma_group_name = NULL;
  libgamma_perror("  Expecting 'LIBGAMMA_DEVICE_REQUIRE_GROUP: 10'", LIBGAMMA_DEVICE_REQUIRE_GROUP);
  printf("\n");
  printf("Testing error code uniqueness: ");
  for (i = -1; i >= LIBGAMMA_ERROR_MIN; i--)
    if (libgamma_value_of_error(libgamma_name_of_error(i)) != i)
      {
	printf("failed\n");
	goto not_unique;
      }
  printf("passed\n");
 not_unique:
  printf("\n");
}


static int select_monitor(libgamma_site_state_t* restrict site_state,
			  libgamma_partition_state_t* restrict part_state,
			  libgamma_crtc_state_t* restrict crtc_state)
{
  int method;
  char* site;
  char* tmp;
  char buf[256];
  int r;
  
  printf("Select adjustment method:\n");
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    printf("    %i:  %s\n", method, method_name(method));
  printf("> ");
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  method = atoi(buf);
  
  printf("Select site: ");
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  tmp = strchr(buf, '\n');
  if (tmp != NULL)
    *tmp = '\0';
  if (buf[0] == '\0')
    site = NULL;
  else
    {
      site = malloc((strlen(buf) + 1) * sizeof(char));
      memcpy(site, buf, strlen(buf) + 1);
    }
  
  if ((r = libgamma_site_initialise(site_state, method, site)))
    {
      free(site);
      return libgamma_perror("error", r), 1;
    }
  
  if (site_state->partitions_available == 0)
    {
      libgamma_site_free(site_state);
      return printf("No partitions found\n"), 1;
    }
  
  printf("Select partition [0, %lu]: ", site_state->partitions_available - 1);
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  
  if ((r = libgamma_partition_initialise(part_state, site_state, (size_t)atoll(buf))))
    {
      libgamma_site_free(site_state);
      return libgamma_perror("error", r), 1;
    }
  
  if (part_state->crtcs_available == 0)
    {
      libgamma_partition_free(part_state);
      libgamma_site_free(site_state);
      return printf("No CRTC:s found\n"), 1;
    }
  
  printf("Select CRTC [0, %lu]: ", part_state->crtcs_available - 1);
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  
  if ((r = libgamma_crtc_initialise(crtc_state, part_state, (size_t)atoll(buf))))
    {
      libgamma_partition_free(part_state);
      libgamma_site_free(site_state);
      return libgamma_perror("error", r), 1;
    }
  
  printf("\n");
  return 0;
}


#define print_crtc_information_(type, notation)								   \
  static void print_crtc_information_##type(int do_print, const char* description, int error, type value)  \
  {													   \
    char buf[256];											   \
    if (do_print)											   \
      {													   \
	if (error)											   \
	  {												   \
	    snprintf(buf, sizeof(buf) / sizeof(char), "  (error) %s", description); 			   \
	    libgamma_perror(buf, error);								   \
	  }												   \
	else												   \
	  printf("  %s: %" notation "\n", description, value);						   \
      }													   \
  }

typedef const char* str;
print_crtc_information_(size_t, "lu")
print_crtc_information_(signed, "i")
print_crtc_information_(int, "i")
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif
print_crtc_information_(float, "f")
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
print_crtc_information_(str, "s")


static void crtc_information(libgamma_crtc_state_t* restrict crtc)
{
  libgamma_method_capabilities_t caps;
  libgamma_crtc_information_t info;
  int fields;
  int field;
  
  libgamma_method_capabilities(&caps, crtc->partition->site->method);
  
  for (fields = caps.crtc_information; field = fields & -fields, fields; fields ^= field)
    {
      if (libgamma_get_crtc_information(&info, crtc, field))
	printf("Could not read CRTC information field %i\n", field);
      free(info.edid);
      free(info.connector_name);
    }
  
  fields = caps.crtc_information;
  if (libgamma_get_crtc_information(&info, crtc, fields))
    printf("An error occurred while reading CRTC information\n");

#define print2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, ERROR_VAR)  \
  print_crtc_information_##TYPE(fields & FIELD_ID, DESCRIPTION, info.ERROR_VAR, info.FIELD_VAR);
#define print(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR)  \
  print2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, FIELD_VAR##_error);
  
  printf("CRTC information:\n");
  if ((fields & LIBGAMMA_CRTC_INFO_EDID))
    {
      if (info.edid_error)
	libgamma_perror("  (error) EDID", info.edid_error);
      else
	{
	  char* edid = libgamma_behex_edid(info.edid, info.edid_length);
	  printf("  EDID: %s\n", edid);
	  printf("  EDID (length): %lu\n", info.edid_length);
	  free(edid);
	}
    }
  print(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM, "width", width_mm);
  print(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM, "height", height_mm);
  print(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID, "width per EDID", width_mm_edid);
  print(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID, "height per EDID", height_mm_edid);
  print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "red gamma ramp size", red_gamma_size, gamma_size_error);
  print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "green gamma ramp size", green_gamma_size, gamma_size_error);
  print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "blue gamma ramp size", blue_gamma_size, gamma_size_error);
  print(signed, LIBGAMMA_CRTC_INFO_GAMMA_DEPTH, "gamma ramp depth", gamma_depth);
  print(int, LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT, "gamma support", gamma_support);
  /* print(size_t, LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER, "subpixel order", subpixel_order); */
  print(int, LIBGAMMA_CRTC_INFO_ACTIVE, "active", active);
  print(str, LIBGAMMA_CRTC_INFO_CONNECTOR_NAME, "connector name", connector_name);
  /* LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE connector_type */
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "red gamma characteristics", gamma_red, gamma_error);
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "green gamma characteristics", gamma_green, gamma_error);
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "blue gamma characteristics", gamma_blue, gamma_error);
  
#undef print
  
  free(info.edid);
  free(info.connector_name);
}


int main(void)
{
  libgamma_site_state_t* restrict site_state = malloc(sizeof(libgamma_site_state_t));
  libgamma_partition_state_t* restrict part_state = malloc(sizeof(libgamma_partition_state_t));
  libgamma_crtc_state_t* restrict crtc_state = malloc(sizeof(libgamma_crtc_state_t));
  
  list_methods_lists();
  method_availability();
  list_default_sites();
  method_capabilities();
  error_test();
  
  if (select_monitor(site_state, part_state, crtc_state))
    return 1;
  
  crtc_information(crtc_state);
  
  libgamma_crtc_free(crtc_state);
  libgamma_partition_free(part_state);
  libgamma_site_free(site_state);
  return 0;
}

