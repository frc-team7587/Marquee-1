/*
 * ColorTable.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 *
 * A list of colors used when cycling through colors. These are known as
 * the "canonical colors.
 */

#ifndef COLORTABLE_H_
#define COLORTABLE_H_

#include "FastLED.h"

#define NUMBER_OF_CANONICAL_COLORS 16

static const CRGB canonical_colors[NUMBER_OF_CANONICAL_COLORS] = {
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::White,
  CRGB::AliceBlue,
  CRGB::ForestGreen,
  CRGB::Lavender,
  CRGB::MistyRose,
  CRGB::DarkOrchid,
  CRGB::DarkOrange,
  CRGB::Teal,
  CRGB::Violet,
  CRGB::Lime,
  CRGB::Chartreuse,
  CRGB::BlueViolet,
  CRGB::Aqua
};

#endif /* COLORTABLE_H_ */
