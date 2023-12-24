/*
 * Marquee.cpp
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 */

#include "Marquee.h"

#include "FastLED.h"

static const CRGB RIPPLE_TABLE[] = {
  {
    .red = 12,
    .green = 0,
    .blue = 48,
  },
  {
    .red = 24,
    .green = 0,
    .blue = 36,
  },
  {
    .red = 36,
    .green = 0,
    .blue = 24,
  },
  {
    .red = 48,
    .green = 0,
    .blue = 12,
  },
  {
    .red = 60,
    .green = 0,
    .blue = 0,
  },
  {
    .red = 48,
    .green = 12,
    .blue = 0,
  },
  {
    .red = 36,
    .green = 24,
    .blue = 0,
  },
  {
    .red = 24,
    .green = 36,
    .blue = 0,
  },
  {
    .red = 12,
    .green = 48,
    .blue = 0,
  },
  {
    .red = 0,
    .green = 60,
    .blue = 0,
  },
  {
    .red = 0,
    .green = 48,
    .blue = 12,
  },
  {
    .red = 0,
    .green = 36,
    .blue = 24,
  },
  {
    .red = 0,
    .green = 24,
    .blue = 36,
  },
  {
    .red = 0,
    .green = 12,
    .blue = 48,
  },
  {
    .red = 0,
    .green = 0,
    .blue = 60,
  },
};

#define RIPPLE_TABLE_LENGTH (sizeof(RIPPLE_TABLE) / sizeof(RIPPLE_TABLE[0]))

Marquee::  Marquee(CRGB *leds, size_t num_leds, const Panel& panel) :
    leds(leds),
    num_leds(num_leds),
    panel(panel) {
}

Marquee::~Marquee() {
}

void Marquee::set_pixel(int led_offset, const CRGB *color) {
  if (0 <= led_offset && led_offset < num_leds) {
    leds[led_offset] = *color;
  }
}

void Marquee::flood(const CRGB *color) {
  for (int i = 0; i < num_leds; ++i) {
    leds[i] = *color;
  }
}

uint16_t Marquee::place_char(
    const CRGB *color,
    uint16_t start_char_column,
    uint16_t row,
    uint16_t column,
    char char_to_place,
    const TypeFace &type_face) {
  const uint16_t columns_in_panel = columns();
  const uint16_t rows_in_panel = rows();

  const uint16_t columns_in_char = type_face.char_width();
  const uint16_t rows_in_char = type_face.char_height();

  const uint16_t max_columns_to_place = columns_in_char - start_char_column;

  const uint16_t actual_columns_to_place =
      (max_columns_to_place + column) < columns_in_panel
      ? max_columns_to_place
      : columns_in_panel - column;

  const uint16_t actual_rows_to_place =
      row + rows_in_char < rows_in_panel
          ? rows_in_char
          : rows_in_panel - row;

  for (uint16_t character_column = 0; character_column < actual_columns_to_place; ++character_column) {
    for (uint16_t character_row = 0; character_row < actual_rows_to_place; ++character_row) {
      if (type_face.char_bit_at(char_to_place, character_row, character_column + start_char_column)) {
        set_pixel(character_row + row, character_column + column, color);
      }
    }
  }
  return column + actual_columns_to_place;
}

uint16_t Marquee::ripple(uint16_t offset) {
  int max_row = rows();
  int max_column = columns();
  uint16_t current_color_table_offset = offset % RIPPLE_TABLE_LENGTH;
  for (int current_column = 0; current_column < max_column; ++current_column) {
    const CRGB *current_color = RIPPLE_TABLE + current_color_table_offset;
    for (int current_row = 0; current_row < max_row; ++current_row) {
      set_pixel(current_row, current_column, current_color);
    }
    current_color_table_offset =
        (++current_color_table_offset) % RIPPLE_TABLE_LENGTH;
  }
  return (++offset) % RIPPLE_TABLE_LENGTH;
}

void Marquee::set_pixel(int row, int column, const CRGB *color) {
  int index = panel.index(row, column);
  set_pixel(index, color);
}

void Marquee::show() {
  vTaskDelay(pdMS_TO_TICKS(1));
  FastLED.show();
}
