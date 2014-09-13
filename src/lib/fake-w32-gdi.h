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


/**
 * One of the Windows `typedef`:s for unsigned 16-bit integer.
 * This is the one used in these functions.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#WORD
 */
typedef uint16_t WORD;

/**
 * One of the Windows `typedef`:s for unsigned 32-bit integer.
 * This is the one used in these functions.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#DWORD
 */
typedef uint32_t DWORD ;
/* XXX Documented to be 32-bit but defined as `typedef unsigned long DWORD`, which is it? */

/**
 * One of the Windows `typedef` for booleanic values, the `int` variant.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#BOOL
 */
typedef int BOOL;

/**
 * Windows `typedef` for a device context (the 'h' stands for 'handle'.)
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#HDC
 */
typedef void* HDC;

/**
 * Windows `typedef` for a window (the 'h' stands for 'handle'.)
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#HWND
 */
typedef void* HWND;

/**
 * One of the Windows `typedef`:s for `void*`, a generic pointer.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#LPVOID
 */
typedef void* LPVOID;

/**
 * A silly Windows `typedef` for a `const char*`, a constant string.
 * It is also defined as NUL-terminated, but so are all C-strings
 * and it makes no difference. However if `UNICODE` were to be defined
 * it would be `const wchar_t*`.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#LPCTSTR
 */
typedef const char* LPCTSTR;

/**
 * Even more silly. A Windows `typedef` for either `wchar_t` or
 * `char`, depending on whether `UNICODE` is defined. We will
 * assume `UNICODE` is not definied because that is just silly.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#TCHAR
 */
typedef char TCHAR;

/**
 * Apperently we need `TRUE` to be defined to understand code,
 * at least in Windows...
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#BOOL
 */
#define TRUE  1

/**
 * Apperently we need `FALSE` to be defined to understand code,
 * at least in Windows...
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx#BOOL
 */
#define FALSE 0



/**
 * Get the device context for a window or the entire screen.
 * 
 * @param   hWnd  The window, `NULL` for the entire screen.
 * @return        The device context.
 * @see           http://msdn.microsoft.com/en-us/library/windows/desktop/dd144871(v=vs.85).aspx
 */
HDC GetDC(HWND hWnd);

/**
 * Free a device context.
 * 
 * @param   hWnd  The window whose device context is `hDC`, `NULL` for the entire screen.
 * @param   hDC   The device context to free.
 * @return        Whether the call was successful.
 * @see           http://msdn.microsoft.com/en-us/library/windows/desktop/dd162920(v=vs.85).aspx
 */
int ReleaseDC(HWND hWnd, HDC hDC);


/**
 * Get information (capabilities) for a device context.
 * 
 * @param   hDC     The device context.
 * @param   nIndex  The information to retrieve, may be `COLORMGMTCAPS`.
 * @return          The details of the queried information, can return `CM_GAMMA_RAMP`
 *                  if `nIndex` is `COLORMGMTCAPS`.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx
 */
int GetDeviceCaps(HDC hDC, int nIndex) __attribute__((const));

/**
 * Colour management capabilities
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx
 */
#define COLORMGMTCAPS 1

/**
 * Colour management capabilities: has gamma ramps
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx
 */
#define CM_GAMMA_RAMP 1


/**
 * Set the gamma ramps for a device.
 * 
 * @param   hDC     The device context.
 * @param   lpRamp  The gamma ramps joined in the order: red, green, blue.
 *                  This is a `WORD*` casted to `void*`.
 * @return          Whether the call was successful.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx
 */
BOOL SetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp);

/**
 * Get the current gamma ramps for a device.
 * 
 * @param   hDC     The device context.
 * @param   lpRamp  The output for the gamma ramps joined in the order: red, green, blue.
 *                  This is a `WORD*` casted to `void*`.
 * @return          Whether the call was successful.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd316946(v=vs.85).aspx
 */
BOOL GetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp);


/**
 * Get the context for a device
 * 
 * @param   lpszDriver  The driver or a display device, use "DISPLAY" if you want a display.
 * @param   lpszDevice  The name of the device. If you want a display use can use the member
 *                      name `DeviceName` in the third parameter, an output parameter, of
 *                      `EnumDisplayDevices`.
 * @param   lpszOutput  We will always use `NULL` here.
 * @param   lpInitData  We will always use `NULL` here. This should actually by a `const DEVMODE*`.
 * @return              The context for the device.
 * @see                 http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx
 */
HDC CreateDC(LPCTSTR restrict lpszDriver, LPCTSTR restrict lpszDevice,
	     LPCTSTR restrict lpszOutput, const void* restrict lpInitData);

/**
 * This macro does not seem to have an official documentation that is correct.
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/dd374074(v=vs.85).aspx
 */
#define TEXT(X) ((LPCTSTR)(X))


/**
 * Information about a display device.
 * 
 * @see  http://msdn.microsoft.com/en-us/library/windows/desktop/dd183569(v=vs.85).aspx
 */
typedef struct
{
  /**
   * Set this to `sizeof(DISPLAY_DEVICE)`.
   */
  DWORD cb;
  
  /**
   * The name of the device.
   */
  TCHAR DeviceName[32];
  
  /**
   * The status of the device, can include `DISPLAY_DEVICE_ACTIVE`.
   */
  DWORD StateFlags;
  
} DISPLAY_DEVICE;

/**
 * Appearently we are incapable of putting asterisks at the
 * end of types names, so istead we preprend them with 'P'.
 * 
 * @see http://msdn.microsoft.com/en-us/library/windows/desktop/dd183569(v=vs.85).aspx
 */
typedef DISPLAY_DEVICE* PDISPLAY_DEVICE;

/**
 * The monitor is "on".
 * 
 * @see http://msdn.microsoft.com/en-us/library/windows/desktop/dd183569(v=vs.85).aspx
 */
#define DISPLAY_DEVICE_ACTIVE 1

/**
 * Get a display device by its name or index.
 * 
 * @param   lpDevice         The name of the device, use `NULL` to base the call on `iDevNum` instead.
 * @param   iDevNum          The index of the device.
 * @param   lpDisplayDevice  Output for the found device.
 * @param   dwFlags          Flags, we will always use zero.
 * @return                   Whether the call was successful. Zero is also returned if `iDevNum`
 *                           is greater than the largest device index on the system.
 * @see                      http://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx
 */
BOOL EnumDisplayDevices(LPCTSTR restrict lpDevice, DWORD iDevNum,
			PDISPLAY_DEVICE restrict lpDisplayDevice, DWORD dwFlags);



#ifndef __GCC__
# undef __attribute__
#endif

#endif

