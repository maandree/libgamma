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

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd144871(v=vs.85).aspx */
HDC GetDC(HWND hWnd)
{
  (void) hWnd;
  return (HDC*)16;
}

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd162920(v=vs.85).aspx */
int ReleaseDC(HWND hWnd, HDC hDC)
{
  (void) hWnd;
  (void) hDC;
  return 1;
}


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx */
int GetDeviceCaps(HDC hDC, int nIndex)
{
  (void) hDC;
  return CM_GAMMA_RAMP + nIndex - COLORMGMTCAPS;
}

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx */
BOOL SetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  (void) hDC;
  (void) lpRamp;
  return TRUE;
}

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd316946(v=vs.85).aspx */
BOOL GetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  (void) hDC;
  (void) lpRamp;
  return TRUE;
}


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx */
HDC CreateDC(LPCTSTR restrict lpszDriver, LPCTSTR restrict lpszDevice,
	     void* restrict lpszOutput, void* restrict lpInitData)
{
  (void) lpszOutput;
  (void) lpInitData;
  (void) lpszDevice;
  if (strcmp(lpszDriver, "DISPLAY"))
    return NULL;
  return (HDC*)16;
}


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx */
BOOL EnumDisplayDevices(LPCTSTR lpDevice, restrict DWORD iDevNum,
			PDISPLAY_DEVICE restrict lpDisplayDevice, DWORD dwFlags)
{
  (void) dwFlags;
  if (lpDevice != NULL)
    {
      fprintf(stderr, "lpDevice (argument 1) for EnumDisplayDevices should be NULL\n");
      abort();
      return FALSE;
    }
  if (iDevNum >= 2)
    return FALSE;
  if (lpDisplayDevice->cb != sizeof(DISPLAY_DEVICE))
    {
      fprintf(stderr,
	      "lpDisplayDevice->cb for EnumDisplayDevices is not sizeof(DISPLAY_DEVICE)\n");
      abort();
      return FALSE;
    }
  strcmp(lpDisplayDevice->DeviceName, "some monitor");
  lpDisplayDevice->StateFlags = DISPLAY_DEVICE_ACTIVE;
  return TRUE;
}

#else


#include <xcb/xcb.h>
#include <xcb/randr.h>


#define GAMMA_RAMP_SIZE  256


static xcb_connection_t* restrict connection = NULL;
static size_t dc_count = 0;
static ssize_t crtc_count = -1;
static xcb_randr_crtc_t* restrict crtcs = NULL;
static xcb_randr_get_screen_resources_current_reply_t* restrict res_reply = NULL;


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd144871(v=vs.85).aspx */
HDC GetDC(HWND hWnd)
{
  (void) hWnd;
  return CreateDC(TEXT("DISPLAY"), "0", NULL, NULL);
}


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd162920(v=vs.85).aspx */
int ReleaseDC(HWND hWnd, HDC hDC)
{
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


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd144877(v=vs.85).aspx */
int GetDeviceCaps(HDC hDC, int nIndex)
{
  (void) hDC;
  return CM_GAMMA_RAMP + nIndex - COLORMGMTCAPS;
}


# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Waggregate-return"

/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd372194(v=vs.85).aspx */
BOOL SetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  xcb_void_cookie_t gamma_cookie =
    xcb_randr_set_crtc_gamma_checked(connection, *(xcb_randr_crtc_t*)hDC, GAMMA_RAMP_SIZE,
				     ((uint16_t*)lpRamp) + 0 * GAMMA_RAMP_SIZE,
				     ((uint16_t*)lpRamp) + 1 * GAMMA_RAMP_SIZE,
				     ((uint16_t*)lpRamp) + 2 * GAMMA_RAMP_SIZE);
  xcb_generic_error_t* error = xcb_request_check(connection, gamma_cookie);
  return error == NULL ? TRUE : FALSE;
}


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd316946(v=vs.85).aspx */
BOOL GetDeviceGammaRamp(HDC hDC, LPVOID restrict lpRamp)
{
  xcb_randr_get_crtc_gamma_cookie_t gamma_cookie;
  xcb_randr_get_crtc_gamma_reply_t* restrict gamma_reply;
  xcb_generic_error_t* error;
  
  gamma_cookie = xcb_randr_get_crtc_gamma(connection, *(xcb_randr_crtc_t*)hDC);
  gamma_reply = xcb_randr_get_crtc_gamma_reply(connection, gamma_cookie, &error);
  
  if (error)
    return FALSE;
  
#define DEST_RAMP(I)  (((uint16_t*)lpRamp) + (I) * GAMMA_RAMP_SIZE)
#define SRC_RAMP(C)  (xcb_randr_get_crtc_gamma_##C(gamma_reply))
  
  memcpy(DEST_RAMP(0), SRC_RAMP(red),   GAMMA_RAMP_SIZE * sizeof(uint16_t));
  memcpy(DEST_RAMP(1), SRC_RAMP(green), GAMMA_RAMP_SIZE * sizeof(uint16_t));
  memcpy(DEST_RAMP(2), SRC_RAMP(blue),  GAMMA_RAMP_SIZE * sizeof(uint16_t));
  
#undef SRC_RAMP
#undef DEST_RAMP
  
  free(gamma_reply);
  return TRUE;
}


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd183490(v=vs.85).aspx */
HDC CreateDC(LPCTSTR restrict lpszDriver, LPCTSTR restrict lpszDevice,
	     void* restrict lpszOutput, void* restrict lpInitData)
{
  int crtc_index = atoi(lpszDevice);
  
  (void) lpszOutput;
  (void) lpInitData;
  
  if (strcmp(lpszDriver, "DISPLAY"))
    return NULL;
  
  if (dc_count == 0) {
    xcb_generic_error_t* error;
    xcb_screen_iterator_t iter;
    xcb_randr_get_screen_resources_current_cookie_t res_cookie;
    
    connection = xcb_connect(NULL, NULL);
    
    iter = xcb_setup_roots_iterator(xcb_get_setup(connection));
    res_cookie = xcb_randr_get_screen_resources_current(connection, iter.data->root);
    res_reply = xcb_randr_get_screen_resources_current_reply(connection, res_cookie, &error);
    
    if (error)
      {
	xcb_disconnect(connection);
	crtc_count = -1;
	return NULL;
      }
    
    crtc_count = res_reply->num_crtcs;
    crtcs = xcb_randr_get_screen_resources_current_crtcs(res_reply);
  }
  
  if (crtc_index >= crtc_count)
    {
      if (dc_count == 0)
	{
	  xcb_disconnect(connection);
	  crtc_count = -1;
	}
      return NULL;
    }
  
  dc_count++;
  return crtcs + crtc_index;
}

# pragma GCC diagnostic pop


/* http://msdn.microsoft.com/en-us/library/windows/desktop/dd162609(v=vs.85).aspx */
BOOL EnumDisplayDevices(LPCTSTR restrict lpDevice, DWORD iDevNum,
			PDISPLAY_DEVICE restrict lpDisplayDevice, DWORD dwFlags)
{
  size_t count = (size_t)crtc_count;
  (void) dwFlags;
  if (lpDevice != NULL)
    {
      fprintf(stderr, "lpDevice (argument 1) for EnumDisplayDevices should be NULL\n");
      abort();
      return FALSE;
    }
  if (crtc_count < 0)
    {
      if (GetDC(NULL) == NULL)
	return FALSE;
      dc_count = 0;
      count = (size_t)crtc_count;
      ReleaseDC(NULL, NULL);
    }
  if (iDevNum >= count)
    return FALSE;
  if (lpDisplayDevice->cb != sizeof(DISPLAY_DEVICE))
    {
      fprintf(stderr, "lpDisplayDevice->cb for EnumDisplayDevices is not sizeof(DISPLAY_DEVICE)\n");
      abort();
      return FALSE;
    }
  sprintf(lpDisplayDevice->DeviceName, "%i", iDevNum);
  lpDisplayDevice->StateFlags = DISPLAY_DEVICE_ACTIVE;
  return TRUE;
}


#endif

