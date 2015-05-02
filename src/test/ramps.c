/**
 * libgamma -- Display server abstraction layer for gamma ramp adjustments
 * Copyright (C) 2014, 2015  Mattias Andrée (maandree@member.fsf.org)
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
#include "ramps.h"


/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁸ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2⁸ − 1] integer output value.
 */
uint8_t invert_ramps8(float encoding)
{
  double i_encoding = (double)(1.f - encoding);
  double f_output = ((double)UINT8_MAX) * i_encoding;
  uint8_t output = (uint8_t)f_output;
  if ((i_encoding < (double)(0.25f)) && (output > UINT8_MAX / 2))
    output = 0;
  if ((i_encoding > (double)(0.75f)) && (output < UINT8_MAX / 2))
    output = UINT8_MAX;
  return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2¹⁶ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2¹⁶ − 1] integer output value.
 */
uint16_t invert_ramps16(float encoding)
{
  double i_encoding = (double)(1.f - encoding);
  double f_output = ((double)UINT16_MAX) * i_encoding;
  uint16_t output = (uint16_t)f_output;
  if ((i_encoding < (double)(0.25f)) && (output > UINT16_MAX / 2))
    output = 0;
  if ((i_encoding > (double)(0.75f)) && (output < UINT16_MAX / 2))
    output = UINT16_MAX;
  return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2³² − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2³² − 1] integer output value.
 */
uint32_t invert_ramps32(float encoding)
{
  double i_encoding = (double)(1.f - encoding);
  double f_output = ((double)UINT32_MAX) * i_encoding;
  uint32_t output = (uint32_t)f_output;
  if ((i_encoding < (double)(0.25f)) && (output > UINT32_MAX / 2))
    output = 0;
  if ((i_encoding > (double)(0.75f)) && (output < UINT32_MAX / 2))
    output = UINT32_MAX;
  return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 2⁶⁴ − 1] integer output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 2⁶⁴ − 1] integer output value.
 */
uint64_t invert_ramps64(float encoding)
{
  double i_encoding = (double)(1.f - encoding);
  double f_output = ((double)UINT64_MAX) * i_encoding;
  uint64_t output = (uint64_t)f_output;
  if ((i_encoding < (double)(0.25f)) && (output > UINT64_MAX / 2))
    output = 0;
  if ((i_encoding > (double)(0.75f)) && (output < UINT64_MAX / 2))
    output = UINT64_MAX;
  return output;
}

/**
 * Test mapping function from [0, 1] float encoding value to [0, 1] float output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 1] float output value.
 */
float invert_rampsf(float encoding)
{
  return 1.f - encoding;
}

/**
 * Test mapping function from [0, 1] double precision float encoding
 * value to [0, 1] double precision float output value.
 * 
 * @param   encoding  [0, 1] float encoding value.
 * @return            [0, 1] float output value.
 */
double invert_rampsd(double encoding)
{
  return ((double)1.f) - encoding;
}

