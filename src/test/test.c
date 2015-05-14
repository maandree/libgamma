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
#include "test.h"


/**
 * Test `libgamma`
 * 
 * @return  Non-zero on machine detectable error, this library
 *          may still be faulty if zero is returned.
 */
int main(void)
{
  libgamma_site_state_t* restrict site_state = malloc(sizeof(libgamma_site_state_t));
  libgamma_partition_state_t* restrict part_state = malloc(sizeof(libgamma_partition_state_t));
  libgamma_crtc_state_t* restrict crtc_state = malloc(sizeof(libgamma_crtc_state_t));
  libgamma_crtc_information_t info;
#define X(R)					\
  libgamma_gamma_##R##_t old_##R, R;		\
  libgamma_gamma_##R##_fun* f_##R = invert_##R;
  LIST_RAMPS
#undef X
  size_t i, n;
  int r, rr = 0;
  
  /* Test miscellaneous parts of the library. */
  list_methods_lists();
  method_availability();
  list_default_sites();
  method_capabilities();
  error_test();
  
  /* Select monitor for tests over CRTC:s, partitions and sites. */
  if (select_monitor(site_state, part_state, crtc_state))
    return 1;
  
  /* Test CRTC information functions. */
  crtc_information(crtc_state);
  
  /* Get the sizes of the gamma ramps for the selected CRTC. */
  libgamma_get_crtc_information(&info, crtc_state, LIBGAMMA_CRTC_INFO_GAMMA_SIZE);
  
  /* Create gamma ramps for each depth. */
#define X(R)					\
  old_##R.red_size = info.red_gamma_size;	\
  old_##R.green_size = info.green_gamma_size;	\
  old_##R.blue_size = info.blue_gamma_size;	\
  R = old_##R;					\
  libgamma_gamma_##R##_initialise(&old_##R);	\
  libgamma_gamma_##R##_initialise(&R);
  LIST_RAMPS
#undef X
  
  /* Fill gamma ramps, for each depth, with the CRTC:s current ramps. */
#define X(R)								\
  if ((rr |= r = libgamma_crtc_get_gamma_##R(crtc_state, &old_##R)))	\
    {									\
      libgamma_perror("libgamma_crtc_get_gamma_" #R, r);		\
      goto done;							\
    }									\
  if ((rr |= r = libgamma_crtc_get_gamma_##R(crtc_state, &R)))		\
    {									\
      libgamma_perror("libgamma_crtc_get_gamma_" #R, r);		\
      goto done;							\
    }
  LIST_RAMPS
#undef X
  
  /* Test function assisted gamma ramps setting. */
#define X(R)								\
  /* Dim the monitor for one second and the restore it. */		\
  printf("Inverting monitor output for 1 second... (" #R ")\n");	\
  if ((rr |= r = libgamma_crtc_set_gamma_##R##_f(crtc_state,		\
						 f_##R, f_##R, f_##R)))	\
    libgamma_perror("libgamma_crtc_set_gamma_" #R "_f", r);		\
  sleep(1);								\
  if ((rr |= r = libgamma_crtc_set_gamma_##R(crtc_state, old_##R)))	\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  printf("Done!\n");							\
  /* Sleep for one second, we have more depths to test. */		\
  printf("Sleeping for 1 second...\n");					\
  sleep(1);
  LIST_RAMPS
#undef X
  
  /* Test getting and setting gamma ramps. */
#define X(R)								\
  /* Get the grand size of the gamma ramps. */				\
  n = R.red_size;							\
  n = n > R.green_size ? n : R.green_size;				\
  n = n > R.blue_size ? n : R.blue_size;				\
  /* Print the current gamma ramps. */					\
  printf("Current gamma ramps (" #R "):\n");				\
  for (i = 0; i < n; i++)						\
    {									\
      if (i < R.red_size)    Y(R.red,   "1");  else  printf("      ");	\
      if (i < R.green_size)  Y(R.green, "2");  else  printf("      ");	\
      if (i < R.blue_size)   Y(R.blue,  "4");  else  printf("      ");	\
      printf("\n");							\
    }									\
  printf("\n");								\
  /* Adjust the gamma ramps for dimming the monitor. */			\
  for (i = 0; i < R.red_size + R.green_size + R.blue_size; i++)		\
    R.red[i] /= 2;							\
  /* Dim the monitor for one second and the restore it. */		\
  printf("Dimming monitor for 1 second...\n");				\
  if ((rr |= r = libgamma_crtc_set_gamma_##R(crtc_state, R)))		\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  sleep(1);								\
  if ((rr |= r = libgamma_crtc_set_gamma_##R(crtc_state, old_##R)))	\
    libgamma_perror("libgamma_crtc_set_gamma_" #R, r);			\
  printf("Done!\n");							\
  /* Sleep for one second, we have more depths to test. */		\
  printf("Sleeping for 1 second...\n");					\
  sleep(1);
#define Y(R, C)  printf("  \033[3" C "m%1.8lf\033[00m", (double)(R[i]))
  LIST_FLOAT_RAMPS
#undef Y
#define Y(R, C)  printf("  \033[3" C "m%16llX\033[00m", (uint64_t)(R[i]))
  LIST_INTEGER_RAMPS
#undef Y
#undef X
  
  /* Test order of gamma ramps. */
  memcpy(ramps16.red, old_ramps16.red, ramps16.red_size * sizeof(uint16_t));
  memset(ramps16.green, 0, ramps16.green_size * sizeof(uint16_t));
  memset(ramps16.blue, 0, ramps16.blue_size * sizeof(uint16_t));
  printf("Making the monitor red-only for 1 second...\n");
  if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, ramps16)))
    libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
  sleep(1);
  memset(ramps16.red, 0, ramps16.red_size * sizeof(uint16_t));
  memcpy(ramps16.green, old_ramps16.green, ramps16.green_size * sizeof(uint16_t));
  printf("Making the monitor green-only for 1 second...\n");
  if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, ramps16)))
    libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
  sleep(1);
  memset(ramps16.green, 0, ramps16.green_size * sizeof(uint16_t));
  memcpy(ramps16.blue, old_ramps16.blue, ramps16.blue_size * sizeof(uint16_t));
  printf("Making the monitor green-only for 1 second...\n");
  if ((rr |= r = libgamma_crtc_set_gamma_ramps16(crtc_state, ramps16)))
    libgamma_perror("libgamma_crtc_set_gamma_ramps16", r);
  sleep(1);
  if ((rr |= r = libgamma_crtc_set_gamma_ramps64(crtc_state, old_ramps64)))
    libgamma_perror("libgamma_crtc_set_gamma_ramps64", r);
  printf("Done!\n");
  
  /* TODO Test gamma ramp restore functions. */
  
 done:
  /* Release resouces. */
#define X(R)					\
  libgamma_gamma_##R##_destroy(&R);		\
  libgamma_gamma_##R##_destroy(&old_##R);
  LIST_RAMPS
#undef X
  libgamma_crtc_free(crtc_state);
  libgamma_partition_free(part_state);
  libgamma_site_free(site_state);
  return rr;
}

