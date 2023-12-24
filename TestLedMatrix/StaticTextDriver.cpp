/*
 * StaticTextDriver.cpp
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 */

#include "StaticTextDriver.h"

StaticTextDriver::StaticTextDriver(const TypeFace &type_face) :
    type_face(type_face) {

}

StaticTextDriver::~StaticTextDriver() {
}

BaseType_t StaticTextDriver::begin(const DisplayMessage &displayMessage,
    Marquee *marquee, QueueHandle_t h_queue) {
  size_t led_count = marquee->led_count();
  CRGB background = displayMessage.background;
  CRGB foreground = displayMessage.foreground;

  marquee->flood(&background);

  const unsigned char *p_text = displayMessage.p_text;
  size_t text_length = displayMessage.text_length;
  uint16_t columns_in_string = type_face.char_width() * text_length;
  uint16_t columns_in_marquee = marquee->columns();
  uint16_t columns_to_display =
      columns_in_string < columns_in_marquee ?
          columns_in_string : columns_in_marquee;
  uint16_t rows = type_face.char_height();
  for (uint16_t column = 0; column < columns_to_display; ++column) {
    for (uint16_t row = 0; row < rows; ++row) {
      if (type_face.bit_at(p_text, text_length, row, column)) {
        marquee->set_pixel(row, column, &foreground);
      }
    }
  }
  marquee->show();
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));
}

BaseType_t StaticTextDriver::refresh(const DisplayMessage &displayMessage,
    Marquee *marquee, QueueHandle_t h_queue) {
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));

}
