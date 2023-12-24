/*
 * TypeFace6x8.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 *
 * A 6 pixel wide by 8 pixel tall typeface, a wrapper around font6x8 that
 * provides access to individual pixels (i.e. bits within character
 * definitions).
 */

#ifndef TYPEFACE6X8_H_
#define TYPEFACE6X8_H_

#include "TypeFace.h"

class TypeFace6x8: public TypeFace {
public:
  TypeFace6x8();
  virtual ~TypeFace6x8();

  virtual int16_t char_height() const;
  virtual int16_t char_width() const;

  virtual bool bit_at(
      const unsigned char *p_text,
      size_t text_length,
      int row,
      int column) const;

};

#endif /* TYPEFACE6X8_H_ */
