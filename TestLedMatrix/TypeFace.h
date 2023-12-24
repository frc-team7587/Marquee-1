/*
 * TypeFace.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 *
 * Represents a type face, a.k.a a font
 */

#ifndef TYPEFACE_H_
#define TYPEFACE_H_

#include <Arduino.h>

class TypeFace {
public:
  TypeFace();
  virtual ~TypeFace();

  virtual int16_t char_height() const = 0;
  virtual int16_t char_width() const = 0;

  /**
   * Look up the bit value of a specified pixel in a string.
   *
   * Parameters:
   * -----------
   *
   * Name        Contents
   * ----------- -------------------------------------------------------------
   * p_text      The string to display
   * text_length The string's length in characters, excluding any terminating
   *             '\0'
   * row         Row, where 0 being the lowest row and value increasing with
   *             altitude
   * column      Column, with 0 being the leftmost column in the string and
   *             increasing to the right
   *
   * Returns: true if the specified pixel is on, false if it is off.
   *
   * Note that (0, 0) is the lowest pixel in the first character of the string.
   *
   */
  virtual bool bit_at(
      const unsigned char *p_text,
      size_t text_length,
      uint16_t row,
      uint16_t column) const = 0;

  /**
   * Look up the specified pixel in the specified character
   *
   * Parameters:
   *
   * Name      Contents
   * --------- ----------------------------------------------------------------
   * text_char The character containing the pixels
   * row       Pixel row, with 0 being the character's bottom and increasing
   *           with altitude
   * column    Pixel column, with 0 representing the leftmost column and
   *           increasing to the right
   *
   * Returns: true if the specified pixel is on, false if it is off.
   *
   */
  virtual bool char_bit_at(
      unsigned char text_char,
      uint16_t row,
      uint16_t column) const = 0;

};

#endif /* TYPEFACE_H_ */
