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
#ifndef LIBGAMMA_FAKE_W32_GDI_H
#define LIBGAMMA_FAKE_W32_GDI_H

#ifndef FAKE_LIBGAMMA_METHOD_W32_GDI
# error Including fake-w32-gdi.h without FAKE_LIBGAMMA_METHOD_W32_GDI
#endif


#ifndef __GCC__
# define __attribute__(x)
#endif

#include <stdint.h>


/* http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx */
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int BOOL;
typedef void* HDC;
typedef void* HWND;
typedef void* LPVOID;
typedef const char* LPCTSTR;
typedef char TCHAR;
#define TRUE  1
#define FALSE 0


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd144871(v=vs.85).aspx */
HDC GetDC(HWND hWnd);

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd162920(v=vs.85).aspx */
int ReleaseDC(HWND hWnd, HDC hDC);


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx */
int GetDeviceCaps(HDC hDC, int nIndex) __attribute__((const));
#define COLORMGMTCAPS 1
#define CM_GAMMA_RAMP 1

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx */
BOOL SetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp);

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd316946(v=vs.85).aspx */
BOOL GetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp);


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx */
HDC CreateDC(LPCTSTR restrict lpszDriver, LPCTSTR restrict lpszDevice,
	     void* restrict lpszOutput, void* restrict lpInitData);
#define TEXT(X) ((LPCTSTR)(X))

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd183569(v=vs.85).aspx */
typedef struct
{
  DWORD cb;
  TCHAR DeviceName[32];
  DWORD StateFlags;
} DISPLAY_DEVICE;
typedef DISPLAY_DEVICE* PDISPLAY_DEVICE;
#define DISPLAY_DEVICE_ACTIVE 1

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx */
BOOL EnumDisplayDevices(LPCTSTR restrict lpDevice, DWORD iDevNum,
			PDISPLAY_DEVICE restrict lpDisplayDevice, DWORD dwFlags);



#ifndef __GCC__
# undefine __attribute__
#endif

#endif

