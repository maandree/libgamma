/* See LICENSE file for copyright and license details. */
#include "edid.h"

#include "libgamma-method.h"
#include "libgamma-error.h"

#include <stdint.h>


/*
 * EDID structure revision 1.1:
 *   http://en.wikipedia.org/w/index.php?title=Extended_display_identification_data&oldid=46295569
 * 
 * EDID structure revision 1.3:
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
libgamma_parse_edid(libgamma_crtc_information_t *restrict this, int32_t fields)
{
#define __test_version(edid, major, minor_min, minor_max)\
	((edid)[18] == (major) && (minor_min) <= (edid)[19] && (edid)[19] <= (minor_max))
#define __m(value)\
	(this->edid[index++] != (value))

	int error = 0, checksum = 0;
	size_t i, index = 0;

	/* If the length of the EDID is not 128 bytes, we know that it is not of EDID
	   structure revision 1.0–1.3, and thus we do not support it. Additionally
	   this make sure we do not do segmentation violation on the next test. */
	if (this->edid_length != 128)
		error = LIBGAMMA_EDID_LENGTH_UNSUPPORTED;
	/* Check that the magic number of that of the EDID structure. */
	else if (__m(0x00) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0xFF) || __m(0x00))
		error = LIBGAMMA_EDID_WRONG_MAGIC_NUMBER;
	/* Check that EDID structure revision 1.1–1.3 is used, those are the only
	   version we support. EDID structure revision 1.3 is also by far the most
	   commonly use revision and it is currently the newest revision. We know
	   that parsing works for both revision 1.1 and revision 1.3, because of
	   this we assume it is also correct for revision 1.2. However, we are not
	   assuming this for revision 1.0 which appeared in August 1994 and was
	   replaced by revision 1.1 in April 1996. */
	else if (!__test_version(this->edid, 1, 1, 3))
		error = LIBGAMMA_EDID_REVISION_UNSUPPORTED;

	/* If we have encountered an error, report it for the fields that require
	   the EDID to be parsed. Note that it is not stored for the EDID field
	   itself because it is not considered an error just because we do not
	   support the used version. */
	this->width_mm_edid_error = this->height_mm_edid_error = this->gamma_error = error;

	/* Retrieve the size of the viewport. This is done even if it is not
	   requested because it is not worth it branch. */
	this->width_mm_edid  = (size_t)this->edid[21] * 10;
	this->height_mm_edid = (size_t)this->edid[22] * 10;

	/* Retrieve the monitor's gamma characteristics. */
	if ((fields & LIBGAMMA_CRTC_INFO_GAMMA) && !error) {
		if (this->edid[23] == 0xFF) {
			/* If the gamma charactistics is FFh (3,55) it should be interpreted as not specified. */
			this->gamma_error = LIBGAMMA_GAMMA_NOT_SPECIFIED;
		} else {
			this->gamma_blue = (float)((int)this->edid[23] + 100) / 100.f;
			this->gamma_red = this->gamma_green = this->gamma_blue;
		}
	}

	/* If not error has occurred, calculate and test the checksum.
	   It is not considered an error that the gamma characteristics
	   is left unspecified in the EDID. */
	if (!error) {
		for (i = 0; i < this->edid_length; i++)
			checksum += (int)this->edid[i];
	}
	/* The checksum should be zero. */
	if (checksum & 255) {
		/* Store the error in all fields that require the EDID to be parsed,
		   as well as the EDID field itself. */
		error = LIBGAMMA_EDID_CHECKSUM_ERROR;
		this->edid_error = this->width_mm_edid_error = this->height_mm_edid_error = error;
		/* If the gamma characteristics is not specified, that is kept,
		   and the checksum error is augmented. */
		this->gamma_error = this->gamma_error == LIBGAMMA_GAMMA_NOT_SPECIFIED
				? LIBGAMMA_GAMMA_NOT_SPECIFIED_AND_EDID_CHECKSUM_ERROR : error;
	}

	/* Return whether or not we encountered an error or if
	   the gamma characteristics was requested but is not
	   specified in the monitor's EDID. */
	return error | this->gamma_error;

#undef __m
#undef __test_version
}
