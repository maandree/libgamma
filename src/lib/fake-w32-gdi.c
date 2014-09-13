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
#ifndef HAVE_LIBGAMMA_METHOD_W32_GDI
# error Compiling fake-w32-gdi.c without FAKE_LIBGAMMA_METHOD_W32_GDI
#endif

/* This file very sloppily translates Windows GDI calls to X RandR calls.
 * It should by no means be used, without additional modification, as a
 * part of a compatibility layer. The purpose of this file is only to make
 * it possible to test for logical errors in Windows specific code on
 * a GNU/Linux system under X. */


#include "fake-w32-gdi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef HAVE_LIBGAMMA_METHOD_X_RANDR
/* Use dummy translation. */



/**
 * Get the device context for a window or the entire screen.
 * 
 * @param   hWnd  The windows, `NULL` for the entire screen.
 * @return        The device context.
 * @see           http://msdn.microsoft.com/en-us/library/windows/desktop/dd144871(v=vs.85).aspx
 */
HDC GetDC(HWND hWnd)
{
  /* Just a non-NULL value. */
  (void) hWnd;
  return (HDC*)16;
}


/**
 * Free a device context.
 * 
 * @param   hWnd  The window whose device context is `hDC`, `NULL` for the entire screen.
 * @param   hDC   The device context to free.
 * @return        Whether the call was successful.
 * @see           http://msdn.microsoft.com/en-us/library/windows/desktop/dd162920(v=vs.85).aspx
 */
int ReleaseDC(HWND hWnd, HDC hDC)
{
  /* Always successful. */
  (void) hWnd;
  (void) hDC;
  return 1;
}



/**
 * Get information (capabilities) for a device context.
 * 
 * @param   hDC     The device context.
 * @param   nIndex  The information to retrieve, may be `COLORMGMTCAPS`.
 * @return          The details of the queried information, can return `CM_GAMMA_RAMP`
 *                  if `nIndex` is `COLORMGMTCAPS`.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx
 */
int GetDeviceCaps(HDC hDC, int nIndex)
{
  /* We have gamma ramps if the user asks for colour management capabilities. */
  (void) hDC;
  return CM_GAMMA_RAMP + nIndex - COLORMGMTCAPS;
}


/**
 * Set the gamma ramps for a device.
 * 
 * @param   hDC     The device context.
 * @param   lpRamp  The gamma ramps joined in the order: red, green, blue.
 *                  This is a `WORD*` casted to `void*`.
 * @return          Whether the call was successful.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx
 */
BOOL SetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  /* Always successful. */
  (void) hDC;
  (void) lpRamp;
  return TRUE;
}


/**
 * Get the current gamma ramps for a device.
 * 
 * @param   hDC     The device context.
 * @param   lpRamp  The output for the gamma ramps joined in the order: red, green, blue.
 *                  This is a `WORD*` casted to `void*`.
 * @return          Whether the call was successful.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd316946(v=vs.85).aspx
 */
BOOL GetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  /* Always successful. */
  (void) hDC;
  (void) lpRamp;
  return TRUE;
}



/**
 * Get the context for a device
 * 
 * @param   lpszDriver  The driver or a display device, use "DISPLAY" if you want a display.
 * @param   lpszDevice  The name of the device. If you want a display use can use the member
 *                      name `DeviceName` in the third parameter, an output parameter, of
 *                      `EnumDisplayDevices`.
 * @param   lpszOutput  We will always use `NULL` here.
 * @param   lpInitData  We will always use `NULL` here. This should actually by a `const DEVMODE*`.
 * @return  -
 * @see     http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx
 */
HDC CreateDC(LPCTSTR restrict lpszDriver, LPCTSTR restrict lpszDevice,
	     LPCTSTR restrict lpszOutput, const void* restrict lpInitData)
{
  /* `NULL` if not asking for a CRTC, otherwise a non-NULL value. */
  (void) lpszOutput;
  (void) lpInitData;
  (void) lpszDevice;
  return strcmp(lpszDriver, "DISPLAY") ? NULL : (HDC*)16;
}


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
BOOL EnumDisplayDevices(LPCTSTR lpDevice, restrict DWORD iDevNum,
			PDISPLAY_DEVICE restrict lpDisplayDevice, DWORD dwFlags)
{
  (void) dwFlags;
  /* Check correctness of `lpDevice`. */
  if (lpDevice != NULL)
    {
      fprintf(stderr, "lpDevice (argument 1) for EnumDisplayDevices should be NULL\n");
      abort();
      return FALSE;
    }
  /* Pretend that we have two CRTC:s. */
  if (iDevNum >= 2)
    return FALSE;
  /* Check correctness of `lpDisplayDevice`. */
  if (lpDisplayDevice->cb != sizeof(DISPLAY_DEVICE))
    {
      fprintf(stderr, "lpDisplayDevice->cb for EnumDisplayDevices is not sizeof(DISPLAY_DEVICE)\n");
      abort();
      return FALSE;
    }
  /* Store an arbitrary name for the monitor. */
  strcmp(lpDisplayDevice->DeviceName, "some monitor");
  /* The connector is always enabled. */
  lpDisplayDevice->StateFlags = DISPLAY_DEVICE_ACTIVE;
  return TRUE;
}


#else
/* Use translation to X RandR. */


#include <xcb/xcb.h>
#include <xcb/randr.h>


/**
 * The gamma ramp size that devices will
 * always have in Windows GDI.
 */
#define GAMMA_RAMP_SIZE  256


/**
 * Connection to the X RandR display.
 */
static xcb_connection_t* restrict connection = NULL;

/**
 * Resouces for the screen.
 * We only have one screen, again this is a very sloppy compatibility layer.
 */
static xcb_randr_get_screen_resources_current_reply_t* restrict res_reply = NULL;

/**
 * The number of available CRTC:s, -1 if not known yet.
 */
static ssize_t crtc_count = -1;

/**
 * List of X RandR CRTC:s.
 */
static xcb_randr_crtc_t* restrict crtcs = NULL;

/**
 * The number of opened CRTC:s.
 */
static size_t dc_count = 0;



/**
 * Get the device context for a window or the entire screen.
 * 
 * @param   hWnd  The windows, `NULL` for the entire screen.
 * @return        The device context.
 * @see           http://msdn.microsoft.com/en-us/library/windows/desktop/dd144871(v=vs.85).aspx
 */
HDC GetDC(HWND hWnd)
{
  /* Return the primary CRTC. */
  (void) hWnd;
  return CreateDC(TEXT("DISPLAY"), "0", NULL, NULL);
}


/**
 * Free a device context.
 * 
 * @param   hWnd  The window whose device context is `hDC`, `NULL` for the entire screen.
 * @param   hDC   The device context to free.
 * @return        Whether the call was successful.
 * @see           http://msdn.microsoft.com/en-us/library/windows/desktop/dd162920(v=vs.85).aspx
 */
int ReleaseDC(HWND hWnd, HDC hDC)
{
  /* Disconnect from the RandR display when all monitors have been closed. */
  (void) hWnd;
  (void) hDC;
  dc_count--;
  if (dc_count == 0)
    {
      if (connection != NULL)
	xcb_disconnect(connection);
      connection = NULL;
      free(res_reply);
      res_reply = NULL;
    }
  return 1;
}



/**
 * Get information (capabilities) for a device context.
 * 
 * @param   hDC     The device context.
 * @param   nIndex  The information to retrieve, may be `COLORMGMTCAPS`.
 * @return          The details of the queried information, can return `CM_GAMMA_RAMP`
 *                  if `nIndex` is `COLORMGMTCAPS`.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx
 */
int GetDeviceCaps(HDC hDC, int nIndex)
{
  /* We have gamma ramps if the user asks for colour management capabilities. */
  (void) hDC;
  return CM_GAMMA_RAMP + nIndex - COLORMGMTCAPS;
}


/* xcb violates the rule to never return `struct`:s. */
#ifdef __GCC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Waggregate-return"
#endif


/**
 * Set the gamma ramps for a device.
 * 
 * @param   hDC     The device context.
 * @param   lpRamp  The gamma ramps joined in the order: red, green, blue.
 *                  This is a `WORD*` casted to `void*`.
 * @return          Whether the call was successful.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx
 */
BOOL SetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  /* We assume that our gamma ramps are of the same size
     as used by Windows GDI (we are so sloppy.) */
  xcb_void_cookie_t gamma_cookie =
    xcb_randr_set_crtc_gamma_checked(connection, *(xcb_randr_crtc_t*)hDC, GAMMA_RAMP_SIZE,
				     ((uint16_t*)lpRamp) + 0 * GAMMA_RAMP_SIZE,
				     ((uint16_t*)lpRamp) + 1 * GAMMA_RAMP_SIZE,
				     ((uint16_t*)lpRamp) + 2 * GAMMA_RAMP_SIZE);
  xcb_generic_error_t* error = xcb_request_check(connection, gamma_cookie);
  return error == NULL ? TRUE : FALSE;
}


/**
 * Get the current gamma ramps for a device.
 * 
 * @param   hDC     The device context.
 * @param   lpRamp  The output for the gamma ramps joined in the order: red, green, blue.
 *                  This is a `WORD*` casted to `void*`.
 * @return          Whether the call was successful.
 * @see             http://msdn.microsoft.com/en-us/library/windows/desktop/dd316946(v=vs.85).aspx
 */
BOOL GetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  xcb_randr_get_crtc_gamma_cookie_t gamma_cookie;
  xcb_randr_get_crtc_gamma_reply_t* restrict gamma_reply;
  xcb_generic_error_t* error;
  
  /* Read current gamma ramps. */
  gamma_cookie = xcb_randr_get_crtc_gamma(connection, *(xcb_randr_crtc_t*)hDC);
  gamma_reply = xcb_randr_get_crtc_gamma_reply(connection, gamma_cookie, &error);
  if (error)
    return FALSE;
  
#define DEST_RAMP(I)  (((uint16_t*)lpRamp) + (I) * GAMMA_RAMP_SIZE)
#define SRC_RAMP(C)  (xcb_randr_get_crtc_gamma_##C(gamma_reply))
  
  /* Copy the ramps into the output parameter. (coalesced) */
  memcpy(DEST_RAMP(0), SRC_RAMP(red),   GAMMA_RAMP_SIZE * sizeof(uint16_t));
  memcpy(DEST_RAMP(1), SRC_RAMP(green), GAMMA_RAMP_SIZE * sizeof(uint16_t));
  memcpy(DEST_RAMP(2), SRC_RAMP(blue),  GAMMA_RAMP_SIZE * sizeof(uint16_t));
  
#undef SRC_RAMP
#undef DEST_RAMP
  
  free(gamma_reply);
  return TRUE;
}



/**
 * Get the context for a device
 * 
 * @param   lpszDriver  The driver or a display device, use "DISPLAY" if you want a display.
 * @param   lpszDevice  The name of the device. If you want a display use can use the member
 *                      name `DeviceName` in the third parameter, an output parameter, of
 *                      `EnumDisplayDevices`.
 * @param   lpszOutput  We will always use `NULL` here.
 * @param   lpInitData  We will always use `NULL` here. This should actually by a `const DEVMODE*`.
 * @return  -
 * @see     http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx
 */
HDC CreateDC(LPCTSTR restrict lpszDriver, LPCTSTR restrict lpszDevice,
	     LPCTSTR restrict lpszOutput, const void* restrict lpInitData)
{
  int crtc_index = atoi(lpszDevice);
  
  (void) lpszOutput;
  (void) lpInitData;
  
  /* Check correctness of input. */
  if (strcmp(lpszDriver, "DISPLAY"))
    return NULL;
  
  /* Connect to the display and get screen data if not already done so. */
  if (dc_count == 0)
    {
      xcb_generic_error_t* error;
      xcb_screen_iterator_t iter;
      xcb_randr_get_screen_resources_current_cookie_t res_cookie;
      
      /* Connect to the display. */
      connection = xcb_connect(NULL, NULL);
      /* Get the first screen. */
      iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
      /* Get the resources of the screen. */
      res_cookie = xcb_randr_get_screen_resources_current(connection, iter.data->root);
      res_reply = xcb_randr_get_screen_resources_current_reply(connection, res_cookie, &error);
      if (error)
	{
	  fprintf(stderr, "Failed to open X connection.\n");
	  xcb_disconnect(connection);
	  crtc_count = -1;
	  return NULL;
	}
      
      /* Get the number of CRTC:s. */
      crtc_count = res_reply->num_crtcs;
      /* Get the CRTC ID:s. */
      crtcs = xcb_randr_get_screen_resources_current_crtcs(res_reply);
    }
  
  /* Was the index too high. */
  if (crtc_index >= crtc_count)
    {
      /* Disconnect and release resouces and
	 mark that we do not know the number of
	 available CRTC:s if we have not opened
	 any monitors yet. */
      if (dc_count == 0)
	{
	  xcb_disconnect(connection);
	  free(res_reply);
	  res_reply = NULL;
	  crtc_count = -1;
	}
      return NULL;
    }
  
  /* We have opened a new CRTC. */
  dc_count++;
  /* Return the ID of the CRTC. */
  return crtcs + crtc_index;
}


#ifdef __GCC__
# pragma GCC diagnostic pop
#endif


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
			PDISPLAY_DEVICE restrict lpDisplayDevice, DWORD dwFlags)
{
  size_t count = (size_t)crtc_count;
  (void) dwFlags;
  /* Check the correctness of the input. */
  if (lpDevice != NULL)
    {
      fprintf(stderr, "lpDevice (argument 1) for EnumDisplayDevices should be NULL\n");
      abort();
      return FALSE;
    }
  /* Do we know how many CRTC:s are available? */
  if (crtc_count < 0)
    {
      /* If not open the first CRTC so that will be figured out. */
      if (GetDC(NULL) == NULL)
	return FALSE;
      count = (size_t)crtc_count;
      /* Close the primary monitor that we just closed. */
      ReleaseDC(NULL, NULL);
    }
  /* Check that the request CRTC exists. */
  if (iDevNum >= count)
    return FALSE;
  /* Check the correctness of the input. */
  if (lpDisplayDevice->cb != sizeof(DISPLAY_DEVICE))
    {
      fprintf(stderr, "lpDisplayDevice->cb for EnumDisplayDevices is not sizeof(DISPLAY_DEVICE)\n");
      abort();
      return FALSE;
    }
  /* Store name for the CRTC. */
  sprintf(lpDisplayDevice->DeviceName, "%i", iDevNum);
  /* The connector that the CRTC belongs to is enabled. */
  lpDisplayDevice->StateFlags = DISPLAY_DEVICE_ACTIVE;
  return TRUE;
}


#endif

