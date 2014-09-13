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
#include "crtcinfo.h"

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


int main(void)
{
  /* ramps16 is last because we want to make sure that the gamma ramps are
     preserved exactly on exit, and we assume RandR X is used. */
#define LIST_INTEGER_RAMPS  X(ramps8) X(ramps32) X(ramps64) X(ramps16)
#define LIST_FLOAT_RAMPS  X(rampsf) X(rampsd)
#define LIST_RAMPS  LIST_FLOAT_RAMPS LIST_INTEGER_RAMPS

  libgamma_site_state_t* restrict site_state = malloc(sizeof(libgamma_site_state_t));
  libgamma_partition_state_t* restrict part_state = malloc(sizeof(libgamma_partition_state_t));
  libgamma_crtc_state_t* restrict crtc_state = malloc(sizeof(libgamma_crtc_state_t));
  libgamma_crtc_information_t info;
#define X(R)				\
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
      if (i < R.red_size)    Y(R, red);					\
      else                   printf("      ");				\
      if (i < R.green_size)  Y(R, green);				\
      else                   printf("      ");				\
      if (i < R.blue_size)   Y(R, blue);				\
      else                   printf("      ");				\
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
#define Y(R, C)  printf("  \033[32m%1.8lf\033[00m", (double)(R.C[i]))
  LIST_FLOAT_RAMPS
#undef Y
#define Y(R, C)  printf("  \033[31m%16llX\033[00m", (uint64_t)(R.C[i]))
  LIST_INTEGER_RAMPS
#undef Y
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
  
#undef LIST_FLOAT_RAMPS
#undef LIST_INTEGER_RAMPS
#undef LIST_RAMPS
}

