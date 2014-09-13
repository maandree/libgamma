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
#include "update-warnings.h"
#include "methods.h"
#include "errors.h"

#include <libgamma.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>



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
#ifdef __GCC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif
print_crtc_information_(float, "f")
#ifdef __GCC__
# pragma GCC diagnostic pop
#endif
print_crtc_information_(str, "s")

#undef print_crtc_information_


#define __case(VALUE)  case VALUE: return #VALUE;

static const char* subpixel_order_str(libgamma_subpixel_order_t value)
{
  switch (value)
    {
    __case (LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN)
    __case (LIBGAMMA_SUBPIXEL_ORDER_NONE)
    __case (LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB)
    __case (LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_BGR)
    __case (LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_RGB)
    __case (LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_BGR)
    default:
      return "(unknown)";
    }
}

static const char* connector_type_str(libgamma_connector_type_t value)
{
  switch (value)
    {
    __case (LIBGAMMA_CONNECTOR_TYPE_Unknown)
    __case (LIBGAMMA_CONNECTOR_TYPE_VGA)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVI)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVII)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVID)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVIA)
    __case (LIBGAMMA_CONNECTOR_TYPE_Composite)
    __case (LIBGAMMA_CONNECTOR_TYPE_SVIDEO)
    __case (LIBGAMMA_CONNECTOR_TYPE_LVDS)
    __case (LIBGAMMA_CONNECTOR_TYPE_Component)
    __case (LIBGAMMA_CONNECTOR_TYPE_9PinDIN)
    __case (LIBGAMMA_CONNECTOR_TYPE_DisplayPort)
    __case (LIBGAMMA_CONNECTOR_TYPE_HDMI)
    __case (LIBGAMMA_CONNECTOR_TYPE_HDMIA)
    __case (LIBGAMMA_CONNECTOR_TYPE_HDMIB)
    __case (LIBGAMMA_CONNECTOR_TYPE_TV)
    __case (LIBGAMMA_CONNECTOR_TYPE_eDP)
    __case (LIBGAMMA_CONNECTOR_TYPE_VIRTUAL)
    __case (LIBGAMMA_CONNECTOR_TYPE_DSI)
    __case (LIBGAMMA_CONNECTOR_TYPE_LFP)
    default:
      return "(unknown)";
    }
}

#undef __case


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
	  char* edid_lc = libgamma_behex_edid(info.edid, info.edid_length);
	  unsigned char* edid_raw = libgamma_unhex_edid(edid_lc);
	  char* edid_uc = libgamma_behex_edid_uppercase(edid_raw, info.edid_length);;
	  printf("  EDID: %s\n", edid_lc);
	  printf("  EDID (uppercase): %s\n", edid_uc);
	  printf("  EDID (length): %lu\n", info.edid_length);
	  free(edid_lc);
	  free(edid_raw);
	  free(edid_uc);
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
  if ((fields & LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER))
    {
      if (info.subpixel_order_error)
	libgamma_perror("  (error) subpixel order", info.subpixel_order_error);
      else
	printf("  subpixel order: %s\n", subpixel_order_str(info.subpixel_order));
    }
  print(int, LIBGAMMA_CRTC_INFO_ACTIVE, "active", active);
  print(str, LIBGAMMA_CRTC_INFO_CONNECTOR_NAME, "connector name", connector_name);
  if ((fields & LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE))
    {
      if (info.connector_type_error)
	libgamma_perror("  (error) subpixel order", info.connector_type_error);
      else
	printf("  subpixel order: %s\n", connector_type_str(info.connector_type));
    }
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "red gamma characteristics", gamma_red, gamma_error);
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "green gamma characteristics", gamma_green, gamma_error);
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "blue gamma characteristics", gamma_blue, gamma_error);
  
#undef print
  
  free(info.edid);
  free(info.connector_name);
  printf("\n");
}


int main(void)
{
  /* ramps16 is last because we want to make sure that the gamma ramps are
     preserved exactly on exit, and we assume RandR X is used. */
#define LIST_INTEGER_RAMPS  X(ramps32) X(ramps64) X(ramps16)
#define LIST_FLOAT_RAMPS  X(rampsf) X(rampsd)
#define LIST_RAMPS  LIST_FLOAT_RAMPS LIST_INTEGER_RAMPS
  /* TODO X(ramps8) */

  libgamma_site_state_t* restrict site_state = malloc(sizeof(libgamma_site_state_t));
  libgamma_partition_state_t* restrict part_state = malloc(sizeof(libgamma_partition_state_t));
  libgamma_crtc_state_t* restrict crtc_state = malloc(sizeof(libgamma_crtc_state_t));
  libgamma_crtc_information_t info;
#define X(R)					\
  libgamma_gamma_##R##_t old_##R, R;
  LIST_RAMPS
#undef X
  size_t i, n;
  int r;
  
  list_methods_lists();
  method_availability();
  list_default_sites();
  method_capabilities();
  error_test();
  
  if (select_monitor(site_state, part_state, crtc_state))
    return 1;
  
  crtc_information(crtc_state);
  
  libgamma_get_crtc_information(&info, crtc_state, LIBGAMMA_CRTC_INFO_GAMMA_SIZE);
  
#define X(R)					\
  old_##R.red_size = info.red_gamma_size;	\
  old_##R.green_size = info.green_gamma_size;	\
  old_##R.blue_size = info.blue_gamma_size;	\
  R = old_##R;					\
  libgamma_gamma_##R##_initialise(&old_##R);	\
  libgamma_gamma_##R##_initialise(&R);
  LIST_RAMPS
#undef X
  
#define X(R)							\
  libgamma_crtc_get_gamma_##R(crtc_state, &old_##R);		\
  if (r = libgamma_crtc_get_gamma_##R(crtc_state, &R), r)	\
    {								\
      libgamma_perror("libgamma_crtc_get_gamma_" #R, r);	\
      goto done;						\
    }
  LIST_RAMPS
#undef X
  
#define X(R)								\
  n = R.red_size;							\
  n = n > R.green_size ? n : R.green_size;				\
  n = n > R.blue_size ? n : R.blue_size;				\
  printf("Current gamma ramps (" #R "):\n");				\
  for (i = 0; i < n; i++)						\
    {									\
      if (i < R.red_size)						\
	printf("  \033[31m%1.8lf\033[00m", (double)(R.red[i]));		\
      else								\
	printf("      ");						\
      if (i < R.green_size)						\
	printf("  \033[32m%1.8lf\033[00m", (double)(R.green[i]));	\
      else								\
	printf("      ");						\
      if (i < R.blue_size)						\
	printf("  \033[34m%1.8lf\033[00m", (double)(R.blue[i]));	\
      else								\
	printf("      ");						\
      printf("\n");							\
    }									\
  printf("\n");								\
									\
  for (i = 0; i < R.red_size + R.green_size + R.blue_size; i++)		\
    R.red[i] /= 2;							\
									\
  printf("Dimming monitor for 1 second...\n");				\
  r = libgamma_crtc_set_gamma_##R(crtc_state, R);			\
  if (r)								\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  sleep(1);								\
  r = libgamma_crtc_set_gamma_##R(crtc_state, old_##R);			\
  if (r)								\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  printf("Done!\n");							\
  printf("Sleeping for 1 second...\n");					\
  sleep(1);
  LIST_FLOAT_RAMPS
#undef X
  
#define X(R)								\
  n = R.red_size;							\
  n = n > R.green_size ? n : R.green_size;				\
  n = n > R.blue_size ? n : R.blue_size;				\
  printf("Current gamma ramps (" #R "):\n");				\
  for (i = 0; i < n; i++)						\
    {									\
      if (i < R.red_size)						\
	printf("  \033[31m%16llX\033[00m", (uint64_t)(R.red[i]));	\
      else								\
	printf("      ");						\
      if (i < R.green_size)						\
	printf("  \033[32m%16llX\033[00m", (uint64_t)(R.green[i]));	\
      else								\
	printf("      ");						\
      if (i < R.blue_size)						\
	printf("  \033[34m%16llX\033[00m", (uint64_t)(R.blue[i]));	\
      else								\
	printf("      ");						\
      printf("\n");							\
    }									\
  printf("\n");								\
									\
  for (i = 0; i < R.red_size + R.green_size + R.blue_size; i++)		\
    R.red[i] /= 2;							\
									\
  printf("Dimming monitor for 1 second...\n");				\
  r = libgamma_crtc_set_gamma_##R(crtc_state, R);			\
  if (r)								\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  sleep(1);								\
  r = libgamma_crtc_set_gamma_##R(crtc_state, old_##R);			\
  if (r)								\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  printf("Done!\n");							\
  printf("Sleeping for 1 second...\n");					\
  sleep(1);
  LIST_INTEGER_RAMPS
#undef X
  
  
 done:
#define X(R)					\
  libgamma_gamma_##R##_destroy(&R);		\
  libgamma_gamma_##R##_destroy(&old_##R);
  LIST_RAMPS
#undef X
  
  /* TODO Test gamma ramp restore functions. */
  /* TODO Test _f gamma ramp setters. */
  
  libgamma_crtc_free(crtc_state);
  libgamma_partition_free(part_state);
  libgamma_site_free(site_state);
  return r;
  
#undef LIST_RAMPS
}

