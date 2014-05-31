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
#if LIBGAMMA_METHOD_COUNT != 6
# warning List of adjustment methods is out of date
#endif
	return "(unknown)";
      }
}


int main(void)
{
  int* methods = malloc(LIBGAMMA_METHOD_COUNT * sizeof(int));
  size_t n = libgamma_list_methods(methods, LIBGAMMA_METHOD_COUNT, 0);
  size_t i;
  
  if (n > LIBGAMMA_METHOD_COUNT)
    {
      printf("Warning: you should to recompile the program, libgamma has been updated.\n");
      methods = realloc(methods, n * sizeof(int));
      libgamma_list_methods(methods, n, 0);
    }
  
  for (i = 0; i < n; i++)
    printf("%s\n", method_name(methods[i]));
  
  return 0;
}

