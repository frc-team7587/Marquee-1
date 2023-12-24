/*
 * TypeFace6x8.cpp
 *
 *  Created on: Jul 24, 2022
 *      Author: Eric Mintz
 */

#include "TypeFace6x8.h"
#include "font6x8.h"

TypeFace6x8::TypeFace6x8() {
}

TypeFace6x8::~TypeFace6x8() {
}

int16_t TypeFace6x8::char_height() const {
  return 8;
}

int16_t TypeFace6x8::char_width() const {
  return 6;
}

bool TypeFace6x8::bit_at(
    const unsigned char *p_text,
    size_t text_length,
    uint16_t row,
    uint16_t column) const {
  uint16_t char_index = column / 6;
  uint16_t column_in_char = column % 6;
  unsigned char current_char =
      0 <= char_index && char_index < text_length ?
          p_text[char_index] : ASCII_NONPRINT;
  return char_bit_at(current_char, row, column_in_char);
}

bool TypeFace6x8::char_bit_at(
    unsigned char text_char,
         uint16_t row,
         uint16_t column) const {
  if ((unsigned char) ASCII_NONPRINT < text_char || text_char < ' ') {
    text_char = ASCII_NONPRINT;
  }

  uint8_t column_bits = fontdata[text_char - ASCII_OFFSET][column];
  return (column_bits >> row) & 1;
}
