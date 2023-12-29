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

  const unsigned char *text = displayMessage.p_text;
  size_t text_length = displayMessage.text_length;
  uint16_t columns_in_display = marquee->columns();
  uint16_t characters_to_display = columns_in_display / type_face.char_width();
  if (text_length < characters_to_display) {
    characters_to_display = text_length;
  }
  uint16_t columns_to_display = characters_to_display * type_face.char_width();
  uint16_t starting_column = (columns_in_display - columns_to_display) / 2;
  marquee->place_string(
      &foreground,
      text,
      text_length,
      0,
      columns_to_display,
      0,
      starting_column,
      type_face);

  marquee->show();
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));
}

BaseType_t StaticTextDriver::refresh(const DisplayMessage &displayMessage,
    Marquee *marquee, QueueHandle_t h_queue) {
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));

}
