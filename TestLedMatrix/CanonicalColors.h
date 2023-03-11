/*
 * CanonicalColors.h
 *
 *  Created on: July 31, 2022
 *      Author: Eric Mintz
 *
 * A singleton list of the canonical colors. See ColorTable.h
 */

#ifndef CANONICALCOLORS_H_
#define CANONICALCOLORS_H_
#include <Arduino.h>
#include "FastLED.h"

#define NUMBER_OF_CANONICAL_COLORS 16

class CanonicalColors {
public:

  static const CRGB *color_at(size_t index);
  static size_t number_of_colors();
};

#endif /* CANONICALCOLORS_H_ */
