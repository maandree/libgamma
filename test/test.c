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


int main(void)
{
  int methods[GAMMA_METHOD_COUNT];
  size_t n = libgamma_list_methods(methods, 0);
  size_t i;
  
  for (i = 0; i < n; i++)
    switch (methods[i])
      {
      case GAMMA_METHOD_DUMMY:
	printf("dummy\n");
	break;
      case GAMMA_METHOD_X_RANDR:
	printf("RandR X extension\n");
	break;
      case GAMMA_METHOD_X_VIDMODE:
	printf("VidMode X extension\n");
	break;
      case GAMMA_METHOD_LINUX_DRM:
	printf("Linux DRM\n");
	break;
      case GAMMA_METHOD_W32_GDI:
	printf("Windows GDI\n");
	break;
      case GAMMA_METHOD_QUARTZ_CORE_GRAPHICS:
	printf("Quartz using Core Graphics\n");
	break;
      default:
#if GAMMA_METHOD_COUNT != 6
# warning List of adjustment methods is out of date
#endif
	printf("(unknown)\n");
        break;
      }
  
  return 0;
}

