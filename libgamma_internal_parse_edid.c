/* See LICENSE file for copyright and license details. */
#include "common.h"


/*
 * EDID structure revision 1.1:
 *   http://en.wikipedia.org/w/index.php?title=Extended_display_identification_data&oldid=46295569
 * 
 * EDID structure revision 1.3:
 *   https://en.wikipedia.org/w/index.php?title=Extended_Display_Identification_Data&oldid=805561402#EDID_1.3_data_format
 * 
 * EDID structure revision 1.4:
 *   http://en.wikipedia.org/wiki/Extended_display_identification_data
 */


/**
 * Parse the EDID of a monitor
 * 
 * @param   this    Instance of a data structure to fill with the information about the EDID;
 *                  it must contain the EDID and its length
 * @param   fields  OR:ed identifiers for the information about the EDID that should be parsed;
 *                  fields that do not have to do with EDID are ignored
 * @return          Non-zero on error
 */
int
libgamma_internal_parse_edid(libgamma_crtc_information_t *restrict this, unsigned long long fields)
{
#define __test_version(edid, major, minor_min, minor_max)\
	((edid)[18] == (major) && (minor_min) <= (edid)[19] && (edid)[19] <= (minor_max))
#define __m(value)\
	(this->edid[index++] != (value))

	uint16_t red_x, green_x, blue_x, white_x;
	uint16_t red_y, green_y, blue_y, white_y;
	int error = 0, checksum = 0;
	size_t i, index = 0;

	/* If the length of the EDID is less than 128 bytes, we know that it is
	 * not of EDID structure revision 1.0–1.4, and thus we do not support it;
	 * however, extensions allows it to be longer tha 128 bytes */
	if (this->edid_length < 128)
		error = LIBGAMMA_EDID_LENGTH_UNSUPPORTED;
	/* Check that the magic number of that of the EDID structure */
	else if (__m(0x00) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0x00))
		error = LIBGAMMA_EDID_WRONG_MAGIC_NUMBER;
	/* Check that EDID structure revision 1.1–1.4 is used, those are the only
	 * version we support. EDID structure revisions 1.3 and 1.4 is also by far
	 * the most commonly use revision and it is currently the newest revisions.
	 * We know that parsing works for revisions 1.1, 1.3, and 1.4, because of
	 * this we assume it is also correct for revision 1.2. However, we are not
	 * assuming this for revision 1.0 which appeared in August 1994 and was
	 * replaced by revision 1.1 in April 1996. */
	else if (!__test_version(this->edid, 1, 1, 4))
		error = LIBGAMMA_EDID_REVISION_UNSUPPORTED;

	/* If we have encountered an error, report it for the fields that require
	 * the EDID to be parsed. Note that it is not stored for the EDID field
	 * itself because it is not considered an error just because we do not
	 * support the used version. */
	this->width_mm_edid_error = this->height_mm_edid_error = this->gamma_error = error;
	this->chroma_error = this->white_point_error = error;

	/* Set errors and exit if we have error, especially important if this->edid_length < 128 */
	if (error)
		goto out;

	/* Retrieve the size of the viewport. This is done even if it is not
	 * requested because it is not worth it to branch. */
	this->width_mm_edid  = (size_t)this->edid[21] * 10;
	this->height_mm_edid = (size_t)this->edid[22] * 10;

	/* Retrieve the monitor's gamma characteristics */
	if (fields & LIBGAMMA_CRTC_INFO_GAMMA) {
		if (this->edid[23] == 0xFF) {
			/* If the gamma charactistics is FFh (3,55) it should be interpreted as not specified. */
			this->gamma_error = LIBGAMMA_GAMMA_NOT_SPECIFIED;
		} else {
			this->gamma_blue = (float)((int)this->edid[23] + 100) / 100.f;
			this->gamma_red = this->gamma_green = this->gamma_blue;
		}
	}

	/* Retrieve the monitor's subpixel chromas */
	if (fields & (LIBGAMMA_CRTC_INFO_CHROMA | LIBGAMMA_CRTC_INFO_WHITE_POINT)) {
		red_x   = (((uint16_t)this->edid[25] >> 6) & 2) | ((uint16_t)this->edid[27] << 2);
		red_y   = (((uint16_t)this->edid[25] >> 4) & 2) | ((uint16_t)this->edid[28] << 2);
		green_x = (((uint16_t)this->edid[25] >> 2) & 2) | ((uint16_t)this->edid[29] << 2);
		green_y = (((uint16_t)this->edid[25] >> 0) & 2) | ((uint16_t)this->edid[30] << 2);
		blue_x  = (((uint16_t)this->edid[26] >> 6) & 2) | ((uint16_t)this->edid[31] << 2);
		blue_y  = (((uint16_t)this->edid[26] >> 4) & 2) | ((uint16_t)this->edid[32] << 2);
		white_x = (((uint16_t)this->edid[26] >> 2) & 2) | ((uint16_t)this->edid[33] << 2);
		white_y = (((uint16_t)this->edid[26] >> 6) & 2) | ((uint16_t)this->edid[34] << 2);
		/* Even though the maximum value as encoded is 1023, the values should be divided by 1024 */
		this->red_chroma_x   = (float)red_x   / 1024.f;
		this->red_chroma_y   = (float)red_y   / 1024.f;
		this->green_chroma_x = (float)green_x / 1024.f;
		this->green_chroma_y = (float)green_y / 1024.f;
		this->blue_chroma_x  = (float)blue_x  / 1024.f;
		this->blue_chroma_y  = (float)blue_y  / 1024.f;
		this->white_point_x  = (float)white_x / 1024.f;
		this->white_point_y  = (float)white_y / 1024.f;
	}

	/* Calculate and test the checksum. It is not considered
	 * an error that the gamma characteristics is left
	 * unspecified in the EDID. */
	if (!error)
		for (i = 0; i < 128; i++)
			checksum += (int)this->edid[i];
	/* The checksum should be zero */
	if (checksum & 255) {
		/* Store the error in all fields that require the EDID to be parsed,
		 * as well as the EDID field itself */
		error = LIBGAMMA_EDID_CHECKSUM_ERROR;
	out:
		this->edid_error = this->width_mm_edid_error = this->height_mm_edid_error = error;
		/* If the gamma characteristics is not specified, that is kept,
		 * and the checksum error is augmented. */
		this->gamma_error = this->gamma_error == LIBGAMMA_GAMMA_NOT_SPECIFIED
				? LIBGAMMA_GAMMA_NOT_SPECIFIED_AND_EDID_CHECKSUM_ERROR : error;
	}

	/* Return whether or not we encountered an error or if
	 * the gamma characteristics was requested but is not
	 * specified in the monitor's EDID */
	return error | this->gamma_error;

#undef __m
#undef __test_version
}
