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
  uint16_t characters_to_display = marquee->columns() / type_face.char_width();
  if (characters_to_display < text_length) {
    characters_to_display = text_length;
  }
  marquee->place_string(
      &foreground,
      text,
      text_length,
      0,
      characters_to_display * type_face.char_width(),
      0,
      0,
      type_face);

  marquee->show();
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));
}

BaseType_t StaticTextDriver::refresh(const DisplayMessage &displayMessage,
    Marquee *marquee, QueueHandle_t h_queue) {
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));

}
