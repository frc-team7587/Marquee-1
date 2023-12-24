/*
 * Panels.cpp
 *
 *  Created on: July 16, 2022
 *      Author: Eric Mintz
 */

#include "Panels.h"
#include <Arduino.h>

#define COLUMN_COUNT 32
#define ROW_COUNT 8
#define NUMBER_OF_LEDS (COLUMN_COUNT * ROW_COUNT)

/**
 * An 8 row by 32 column panel where row 0 of even columns is at the bottom
 * and row 0 of odd columns is at the top. The geometry is:
 *
 *   ^
 *   |
 *
 *   Y+
 *   ^
 * 7 | 248 247 232 231    ...    007
 * 6 | 249 246 233 230    ...    006
 * 5 | 250 245 234 229    ...    005
 * 4 | 251 244 235 228    ...    004
 * 3 | 252 243 236 227    ...    003
 * 2 | 253 242 237 226    ...    002
 * 1 | 254 241 238 225    ...    001
 * 0 | 255 240 239 224    ...    000
 *   +------------------------------> X+ -->
 *     0    0   0   0             3
 *     0    1   2   3             1
 *
 *
 */
class EightByThirtyTwoAlternating: public Panel {
//  static const int number_of_leds = 8 * 32;
//  static const int column_count = 32;

  int16_t column_offsets[32];

public:
  EightByThirtyTwoAlternating() {
    int16_t last = NUMBER_OF_LEDS;
    for (int i = 0; i < 32; ++i) {
      last -= ((i & 1) ? 15 : 1);
      column_offsets[i] = last;
    }
  }

  virtual int columns() const {
    return COLUMN_COUNT;
  }

  virtual int led_count() const {
    return NUMBER_OF_LEDS;
  }

  virtual int index(int row, int column, int panel_no) const {
    if (!(column & 1)) {
      row = -row;
    }
    int16_t offset = column_offsets[column] + row;
    int led_offset = NUMBER_OF_LEDS * panel_no + offset;
    return led_offset;
  }

  virtual int rows() const {
    return ROW_COUNT;
  }
};

static EightByThirtyTwoAlternating eight_by_thirty_to_alternating;

const Panel* Panels::alternatingEightByThirtyTwo() {
  return &eight_by_thirty_to_alternating;
}
