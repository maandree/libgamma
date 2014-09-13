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
#include "crtcinfo.h"

#include <stdio.h>
#include <stdlib.h>


/**
 * Conditionally print a CRTC information field.
 * 
 * @param  type:data-type           Data type for the value of the information field.
 * @param  notation:string-literal  %-pattern for `printf` (excluding the %) that is used for `type`.
 * @param  do_print                 Whether the information should be included in the process's output.
 * @param  description              A description of the information field.
 * @param  error                    The error of the information field.
 * @param  value                    The value of the information field.
 */
#define print_crtc_information_(type, notation)								   \
  static void print_crtc_information_##type(int do_print, const char* description, int error, type value)  \
  {													   \
    char buf[256];											   \
    if (do_print)											   \
      {													   \
	if (error)											   \
	  {												   \
	    snprintf(buf, sizeof(buf) / sizeof(char), "  (error) %s", description); 			   \
	    libgamma_perror(buf, error);								   \
	  }												   \
	else												   \
	  printf("  %s: %" notation "\n", description, value);						   \
      }													   \
  }


/**
 * A single [a-z] word alternative to `const char*`, we need it for the
 * function name of the string variant of `print_crtc_information_*`.
 */
typedef const char* str;
/* Create `print_crtc_information_*` variants. */
print_crtc_information_(size_t, "lu")
print_crtc_information_(signed, "i")
print_crtc_information_(int, "i")
#ifdef __GCC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif
print_crtc_information_(float, "f")
#ifdef __GCC__
# pragma GCC diagnostic pop
#endif
print_crtc_information_(str, "s")
#undef print_crtc_information_


#define __case(VALUE)  case VALUE: return #VALUE;

/**
 * Get a string representation of a subpixel order.
 * 
 * @param   value  The subpixel order.
 * @return         String representation.
 */
static const char* subpixel_order_str(libgamma_subpixel_order_t value)
{
  switch (value)
    {
    __case (LIBGAMMA_SUBPIXEL_ORDER_UNKNOWN)
    __case (LIBGAMMA_SUBPIXEL_ORDER_NONE)
    __case (LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_RGB)
    __case (LIBGAMMA_SUBPIXEL_ORDER_HORIZONTAL_BGR)
    __case (LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_RGB)
    __case (LIBGAMMA_SUBPIXEL_ORDER_VERTICAL_BGR)
    default:
      return "(unknown)";
    }
}


/**
 * Get a string representation of a connector type.
 * 
 * @param   value  The connector type.
 * @return         String representation.
 */
static const char* connector_type_str(libgamma_connector_type_t value)
{
  switch (value)
    {
    __case (LIBGAMMA_CONNECTOR_TYPE_Unknown)
    __case (LIBGAMMA_CONNECTOR_TYPE_VGA)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVI)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVII)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVID)
    __case (LIBGAMMA_CONNECTOR_TYPE_DVIA)
    __case (LIBGAMMA_CONNECTOR_TYPE_Composite)
    __case (LIBGAMMA_CONNECTOR_TYPE_SVIDEO)
    __case (LIBGAMMA_CONNECTOR_TYPE_LVDS)
    __case (LIBGAMMA_CONNECTOR_TYPE_Component)
    __case (LIBGAMMA_CONNECTOR_TYPE_9PinDIN)
    __case (LIBGAMMA_CONNECTOR_TYPE_DisplayPort)
    __case (LIBGAMMA_CONNECTOR_TYPE_HDMI)
    __case (LIBGAMMA_CONNECTOR_TYPE_HDMIA)
    __case (LIBGAMMA_CONNECTOR_TYPE_HDMIB)
    __case (LIBGAMMA_CONNECTOR_TYPE_TV)
    __case (LIBGAMMA_CONNECTOR_TYPE_eDP)
    __case (LIBGAMMA_CONNECTOR_TYPE_VIRTUAL)
    __case (LIBGAMMA_CONNECTOR_TYPE_DSI)
    __case (LIBGAMMA_CONNECTOR_TYPE_LFP)
    default:
      return "(unknown)";
    }
}

#undef __case


/**
 * The CRTC information for a CRTC.
 * 
 * @param  crtc  The CRTC.
 */
void crtc_information(libgamma_crtc_state_t* restrict crtc)
{
  libgamma_method_capabilities_t caps;
  libgamma_crtc_information_t info;
  int fields;
  int field;
  
  /* Get supported CRTC informations fields. */
  libgamma_method_capabilities(&caps, crtc->partition->site->method);
  
  /* List unsupport information fields by testing them one by one. */
  for (fields = caps.crtc_information; field = fields & -fields, fields; fields ^= field)
    {
      if (libgamma_get_crtc_information(&info, crtc, field))
	printf("Could not read CRTC information field %i\n", field);
      free(info.edid);
      free(info.connector_name);
    }
  
  /* Get CRTC information, that is supported. */
  fields = caps.crtc_information;
  if (libgamma_get_crtc_information(&info, crtc, fields))
    printf("An error occurred while reading CRTC information\n");
  
  /* Macros for printing CRTC information. */
#define print2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, ERROR_VAR)  \
  print_crtc_information_##TYPE(fields & FIELD_ID, DESCRIPTION, info.ERROR_VAR, info.FIELD_VAR);
#define print(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR)  \
  print2(TYPE, FIELD_ID, DESCRIPTION, FIELD_VAR, FIELD_VAR##_error);
  
  
  /* Print CRTC information. */
  
  printf("CRTC information:\n");
  
  /* Print the EDID field. */
  if ((fields & LIBGAMMA_CRTC_INFO_EDID))
    {
      if (info.edid_error)
	libgamma_perror("  (error) EDID", info.edid_error);
      else
	{
	  char* edid_lc = libgamma_behex_edid(info.edid, info.edid_length);
	  unsigned char* edid_raw = libgamma_unhex_edid(edid_lc);
	  char* edid_uc = libgamma_behex_edid_uppercase(edid_raw, info.edid_length);;
	  printf("  EDID: %s\n", edid_lc);
	  printf("  EDID (uppercase): %s\n", edid_uc);
	  printf("  EDID (length): %lu\n", info.edid_length);
	  free(edid_lc);
	  free(edid_raw);
	  free(edid_uc);
	}
    }
  /* Print physical dimensions of the monitor. */
  print(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM, "width", width_mm);
  print(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM, "height", height_mm);
  print(size_t, LIBGAMMA_CRTC_INFO_WIDTH_MM_EDID, "width per EDID", width_mm_edid);
  print(size_t, LIBGAMMA_CRTC_INFO_HEIGHT_MM_EDID, "height per EDID", height_mm_edid);
  /* Print gamma ramp information. */
  print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "red gamma ramp size", red_gamma_size, gamma_size_error);
  print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "green gamma ramp size", green_gamma_size, gamma_size_error);
  print2(size_t, LIBGAMMA_CRTC_INFO_GAMMA_SIZE, "blue gamma ramp size", blue_gamma_size, gamma_size_error);
  print(signed, LIBGAMMA_CRTC_INFO_GAMMA_DEPTH, "gamma ramp depth", gamma_depth);
  print(int, LIBGAMMA_CRTC_INFO_GAMMA_SUPPORT, "gamma support", gamma_support);
  /* Print subpixel order for the monitor. */
  if ((fields & LIBGAMMA_CRTC_INFO_SUBPIXEL_ORDER))
    {
      if (info.subpixel_order_error)
	libgamma_perror("  (error) subpixel order", info.subpixel_order_error);
      else
	printf("  subpixel order: %s\n", subpixel_order_str(info.subpixel_order));
    }
  /* Print connector information. */
  print(int, LIBGAMMA_CRTC_INFO_ACTIVE, "active", active);
  print(str, LIBGAMMA_CRTC_INFO_CONNECTOR_NAME, "connector name", connector_name);
  if ((fields & LIBGAMMA_CRTC_INFO_CONNECTOR_TYPE))
    {
      if (info.connector_type_error)
	libgamma_perror("  (error) subpixel order", info.connector_type_error);
      else
	printf("  subpixel order: %s\n", connector_type_str(info.connector_type));
    }
  /* Print the gamma characteristics of the monitor. */
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "red gamma characteristics", gamma_red, gamma_error);
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "green gamma characteristics", gamma_green, gamma_error);
  print2(float, LIBGAMMA_CRTC_INFO_GAMMA, "blue gamma characteristics", gamma_blue, gamma_error);
  printf("\n");
  
  
#undef print
#undef print2
  
  /* Release resouces. */
  free(info.edid);
  free(info.connector_name);
}

