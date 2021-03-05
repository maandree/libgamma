/* See LICENSE file for copyright and license details. */
#include "common.h"


#define X(...) +1

/**
 * The number adjustment methods provided by the
 * version this library the program is linked
 * against
 * 
 * This number includes both compile-time enabled
 * and compile-time disabled adjustment methods
 */
const int libgamma_method_count = LIST_METHODS(X);

#undef X
