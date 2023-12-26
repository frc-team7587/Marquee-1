/*
 * DisplayCommand.cpp
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 */

#include "DisplayCommand.h"

static DisplayCommand COMMAND_TABLE[] = {
  ERROR,
  FILL_WITH_COLOR,
  FLASH_TWO_COLORS,
  RIPPLING_RAINBOW,
  SINGLE_PIXEL_NATURAL_ORDER,
  SINGLE_PIXEL_LEFT_TO_RIGHT,
  STATIC_TEXT,
  SCROLLING_TEXT,
};

DisplayCommand DisplayCommandLookup::from_index(uint16_t index) {
  return
      (NUMBER_OF_COMMANDS <= index)
      ? ERROR
      : COMMAND_TABLE[index];
}
