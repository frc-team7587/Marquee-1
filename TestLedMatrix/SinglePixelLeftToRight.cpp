/*
 * SinglePixelLeftToRight.cpp
 *
 *  Created on: Jul 24, 2022
 *      Author: Eric Mintz
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "SinglePixelLeftToRight.h"
#include "ColorTable.h"

CRGB black = CRGB::Black;

SinglePixelLeftToRight::SinglePixelLeftToRight() :
    color_index(0), current_color(CRGB::Red), row(0), column(0) {
}

SinglePixelLeftToRight::~SinglePixelLeftToRight() {
}

BaseType_t SinglePixelLeftToRight::begin(
    const DisplayMessage &displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  row = 0;
  column = 0;
  current_color = CRGB::Red;
  return write_pixel(marquee, h_queue);
}

BaseType_t SinglePixelLeftToRight::refresh(
    const DisplayMessage &displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  return write_pixel(marquee, h_queue);
}

BaseType_t SinglePixelLeftToRight::write_pixel
    (Marquee *marquee,
    QueueHandle_t h_queue) {
  size_t led_count = marquee->led_count();
  marquee->flood(&black);
  marquee->set_pixel(row, column, &current_color);
  marquee->show();
  if (marquee->columns() <= ++column) {
    column = 0;
    if (marquee->rows() <= ++row) {
      row = 0;
      color_index = (++color_index) % NUMBER_OF_CANONICAL_COLORS;
      current_color = canonical_colors[color_index];
    }
  }

  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(20));
}
