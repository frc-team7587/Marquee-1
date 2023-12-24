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

  virtual bool bit_at(const unsigned char *p_text, size_t text_length, int row,
      int column) const = 0;

};

#endif /* TYPEFACE_H_ */
