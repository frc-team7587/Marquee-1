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

bool Marquee::pixel_state_from_string(
    const unsigned char *text,
    uint16_t row,
    uint16_t column,
    const TypeFace &typeface) {
  uint16_t character_width = typeface.char_width();
  uint16_t character_index = column / character_width;
  uint16_t pixel_offset = column % character_width;
  return typeface.char_bit_at(text[character_index], row, pixel_offset);
}

Marquee::Marquee(CRGB *leds, size_t num_leds, const Panel& panel) :
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

void Marquee::place_string(
    const CRGB *color,
    const unsigned char *text,
    size_t text_length,
    uint16_t start_text_column,
    uint16_t number_of_columns,
    uint16_t row,
    uint16_t column,
    const TypeFace &typeface) {
  const uint16_t character_height = typeface.char_height();
  const uint16_t character_width = typeface.char_width();

  uint16_t max_panel_row = row + character_height;
  if (panel.rows() < max_panel_row) {
    max_panel_row = panel.rows();
  }

  uint16_t max_panel_column = column + number_of_columns;
  if (panel.columns() < max_panel_column) {
    max_panel_column = panel.columns();
  }
  const uint16_t text_length_in_columns = text_length * character_width;
  const uint16_t max_text_column = column + text_length_in_columns;
  if (max_text_column < max_panel_column) {
    max_panel_column = max_text_column;
  }

  uint16_t current_text_column = start_text_column;
  for (
      uint16_t current_panel_column = column;
      current_panel_column < max_panel_column;
      ++current_panel_column) {
    uint16_t current_text_row = 0;
    for (
        uint16_t current_panel_row = row;
        current_panel_row < max_panel_row;
        ++current_panel_row) {
      if (pixel_state_from_string(
          text,
          current_text_row++,
          current_text_column,
          typeface)) {
        set_pixel(current_panel_row, current_panel_column, color);
      }
    }
    ++current_text_column;
  }
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
  if (0 <= index && index < panel.led_count()) {
  set_pixel(index, color);
  } else {
    Serial.printf("Pixel (%d, %d) index %d is out of bounds.\n",
        row,
        column,
        index);
    Serial.flush();
  }
}

void Marquee::show() {
  vTaskDelay(pdMS_TO_TICKS(1));
  FastLED.show();
}
