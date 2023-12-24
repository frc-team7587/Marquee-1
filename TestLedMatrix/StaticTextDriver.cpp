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
  size_t chars_in_display = marquee->columns() / type_face.char_width();
  if (chars_in_display < text_length) {
    text_length = chars_in_display;
  }
  uint16_t rows = type_face.char_height();
  size_t display_column = 0;

  for (size_t char_index = 0; char_index < text_length; ++char_index) {
    display_column = marquee->place_char(
        &foreground,
        0,
        0,
        display_column,
        *(p_text++),
        type_face);
  }

  marquee->show();
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));
}

BaseType_t StaticTextDriver::refresh(const DisplayMessage &displayMessage,
    Marquee *marquee, QueueHandle_t h_queue) {
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));

}
