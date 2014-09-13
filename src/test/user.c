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
#include "user.h"


#ifdef __GCC__
# include <string.h>
#else
/* Hack to circumvent that the funcions are defined multiples. */
# define strlen strlen_
# define memcpy memcpy_
# define strchr strchr_
static size_t strlen_(const char* str)
{
  size_t n = 0;
  while (str[n])
    n++;
  return n;
}
static void* memcpy_(void* dest, const void* src, size_t n)
{
  char* restrict d = dest;
  const char* restrict s = src;
  size_t i;
  for (i = 0; i < n; i++)
    d[i] = s[i];
  return dest;
}
static char* strchr_(const char* s, int c)
{
  char find = (char)c;
  while (*s)
    if (*s == find)
      return s;
    else
      s++;
  return NULL;
}
#endif


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
		   libgamma_crtc_state_t* restrict crtc_state)
{
  int method;
  char* site;
  char* tmp;
  char buf[256];
  int r;
  
  
  /* -- Adjustment method -- */
  
  /* Let the user select adjustment method. */
  printf("Select adjustment method:\n");
  for (method = 0; method < LIBGAMMA_METHOD_COUNT; method++)
    printf("    %i:  %s\n", method, method_name(method));
  printf("> ");
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  method = atoi(buf);
  
  
  /* -- Site -- */
  
  /* Let the user select site. */
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
  
  /* Initialise site state. */
  if ((r = libgamma_site_initialise(site_state, method, site)))
    {
      free(site);
      return libgamma_perror("error", r), 1;
    }
  
  
  /* -- Partition -- */
  
  /* Check that the site has at least one partition. */
  if (site_state->partitions_available == 0)
    {
      libgamma_site_free(site_state);
      return printf("No partitions found\n"), 1;
    }
  
  /* Let the user select partition. */
  printf("Select partition [0, %lu]: ", site_state->partitions_available - 1);
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  
  /* Initialise partition state. */
  if ((r = libgamma_partition_initialise(part_state, site_state, (size_t)atoll(buf))))
    {
      libgamma_site_free(site_state);
      return libgamma_perror("error", r), 1;
    }
  
  
  /* -- CRTC -- */
  
  /* Check that the partition has at least one CRTC. */
  if (part_state->crtcs_available == 0)
    {
      libgamma_partition_free(part_state);
      libgamma_site_free(site_state);
      return printf("No CRTC:s found\n"), 1;
    }
  
  /* Let the user select CRTC. */
  printf("Select CRTC [0, %lu]: ", part_state->crtcs_available - 1);
  fflush(stdout);
  fgets(buf, sizeof(buf) / sizeof(char), stdin);
  
  /* Initialise CRTC state. */
  if ((r = libgamma_crtc_initialise(crtc_state, part_state, (size_t)atoll(buf))))
    {
      libgamma_partition_free(part_state);
      libgamma_site_free(site_state);
      return libgamma_perror("error", r), 1;
    }
  
  printf("\n");
  return 0;
}

