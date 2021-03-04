/* See LICENSE file for copyright and license details. */
#include "common.h"


/**
 * Convert an hexadecimal representation of an EDID to a raw representation
 * 
 * @param   edid  The EDID in hexadecimal representation
 * @return        The EDID in raw representation, it will be half the length
 *                of `edid` (the input value); `NULL` on allocation error or
 *                if the EDID is malformated, `errno` will be set accordingly
 */
unsigned char *
libgamma_unhex_edid(const char *restrict edid)
{
	unsigned char *restrict out;
	size_t i, n = strlen(edid);
	char a, b;

	/* Check that the length of the strings is even,
	 * otherwise it cannot represent bytes */
	if (n & 1) {
		errno = EINVAL;
		return NULL;
	}

	/* Allocate memory area for output octet array */
	n /= 2 * sizeof(unsigned char);
	out = malloc(n);
	if (!out)
		return NULL;

	/* Convert to raw octet array */
	for (i = 0; i < n; i++) {
		/* Get the next character pair that, it represents an octet. */
		a = edid[i * 2 + 0];
		b = edid[i * 2 + 1];

		/* Verify that the input is in hexadecimal */
		if (!isxdigit(a) || !isxdigit(b)) {
			free(out);
			errno = EINVAL;
			return NULL;
		}

		/* Convert each chararacter to raw format */
		a = (char)((a & 15) + (a > '9' ? 9 : 0));
		b = (char)((b & 15) + (b > '9' ? 9 : 0));

		/* Combine the two characters into one octet */
		out[i] = (unsigned char)((a << 4) | b);
	}

	return out;
}
