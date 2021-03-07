/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Convert a raw representation of an EDID to a lowercase hexadecimal representation
 * 
 * @param   edid    The EDID in raw representation
 * @param   length  The length of `edid`
 * @return          The EDID in lowercase hexadecimal representation,
 *                  `NULL` on allocation error, `errno` will be set accordingly
 */
char *
libgamma_behex_edid_lowercase(const unsigned char *restrict edid, size_t length)
{
	char *restrict out;
	size_t i;

	/* Allocate memory area for the output string */
	if (length > (SIZE_MAX / sizeof(char) - 1) / 2) {
		errno = ENOMEM;
		return NULL;
	}
	out = malloc((length * 2 + 1) * sizeof(char));
	if (!out)
		return NULL;

	/* Translate from raw octets to hexadecimal */
	for (i = 0; i < length; i++) {
		out[i * 2 + 0] = "0123456789abcdef"[(edid[i] >> 4) & 15];
		out[i * 2 + 1] = "0123456789abcdef"[(edid[i] >> 0) & 15];
	}
	/* NUL-terminate the output string */
	out[length * 2] = '\0';

	return out;
}
