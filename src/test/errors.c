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
#include "errors.h"

#include <libgamma.h>

#include <stdio.h>
#include <errno.h>



/**
 * The error API.
 */
void error_test(void)
{
  int i;
  
  /* Test that naming and name dereferencing
     of errors work. Because the mappings in
     these [`libgamma_value_of_error` and
     `libgamma_name_of_error`] functions are
     generated, it should work if and only if
     one test passes, assumming the errors are
     unique whihc is tested in the end of this
     function. */
  printf("Testing error API using LIBGAMMA_STATE_UNKNOWN:\n");
  printf("  Expecting %i: %i\n", LIBGAMMA_STATE_UNKNOWN, libgamma_value_of_error("LIBGAMMA_STATE_UNKNOWN"));
  printf("  Expecting %s: %s\n", "LIBGAMMA_STATE_UNKNOWN", libgamma_name_of_error(LIBGAMMA_STATE_UNKNOWN));
  printf("\n");
  
  /* Test that `libgamma_perror` can print
     libgamma errors and system errors, and
     handle success in the same with as `perror`. */
  printf("Testing libgamma_perror:\n");
  libgamma_perror("  Expecting LIBGAMMA_STATE_UNKNOWN", LIBGAMMA_STATE_UNKNOWN);
  libgamma_perror("  Expecting a description for ENOMEM", ENOMEM);
  libgamma_perror("  Expecting a description for successfulness", 0);
  /* Test that `libgamma_perror` handles
     `LIBGAMMA_ERRNO_SET` correctly. */
  libgamma_perror("  Expecting a description for ENOMEM", (errno = ENOMEM, LIBGAMMA_ERRNO_SET));
  /* That that `libgamma_perror` handles
     `LIBGAMMA_DEVICE_REQUIRE_GROUP`
     correctly both when the required
     group's name is known and when it
     is unknown. */
  libgamma_group_gid = 10;
  libgamma_group_name = "test";
  libgamma_perror("  Expecting 'LIBGAMMA_DEVICE_REQUIRE_GROUP: test (10)'", LIBGAMMA_DEVICE_REQUIRE_GROUP);
  libgamma_group_name = NULL;
  libgamma_perror("  Expecting 'LIBGAMMA_DEVICE_REQUIRE_GROUP: 10'", LIBGAMMA_DEVICE_REQUIRE_GROUP);
  printf("\n");
  
  /* That all libgamma error codes
     are unique. This is done by
     getting the name associated
     with an error code and the getting
     the error code associated that
     name and test that the same
     error code is returned as put in,
     for each libgamma error code. */
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

