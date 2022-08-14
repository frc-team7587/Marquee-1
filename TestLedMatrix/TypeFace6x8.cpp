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
	int row,
	int column) const {
	int char_index = column/6;
	int column_in_char = column % 6;
	unsigned char current_char = 0 <= char_index && char_index < text_length
		? p_text[char_index]
		: ASCII_NONPRINT;
	if ((unsigned char) ASCII_NONPRINT < current_char || current_char < ' ') {
		current_char = ASCII_NONPRINT;
	}

	uint8_t column_bits = fontdata[current_char - ASCII_OFFSET][column_in_char];
	return (column_bits >> row) & 1;
}

