/*
 * CaninicalColors.cpp
 *
 *  Created on: July 31, 2022
 *      Author: Eric Mintz
 *
 * Single instance canonical color table;
 */

#include "CanonicalColors.h"
#include "ColorTable.h"

size_t CanonicalColors::number_of_colors() {
	return NUMBER_OF_CANONICAL_COLORS;
}

const CRGB *CanonicalColors::color_at(size_t index) {
	return 0 <= index && index < NUMBER_OF_CANONICAL_COLORS
		? canonical_colors + index
		: NULL;
}

