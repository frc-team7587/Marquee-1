/*
 * TextCrawlDriver.cpp
 *
 *  Created on: Dec 24, 2023
 *      Author: Eric Mintz
 */

#include "TextCrawlDriver.h"

TextCrawlDriver::TextCrawlDriver(const TypeFace &type_face) :
    type_face(type_face),
    text(NULL),
    text_length(0),
    delay(0),
    length_in_columns(0),
    foreground(CRGB::Black),
    background(CRGB::Black) {

}

TextCrawlDriver::~TextCrawlDriver() {
}

BaseType_t TextCrawlDriver::advance_display(
    Marquee *marquee,
    QueueHandle_t h_queue) {
  const uint16_t marquee_width = marquee->columns();
  marquee->flood(&background);

  if (start_text_column >= length_in_columns - 1) {
    start_display_column = marquee_width;
    start_text_column = 0;
  }

  if (0 < start_display_column) {
    --start_display_column;
  } else {
    ++start_text_column;
  }

  uint16_t number_of_text_columns_to_display =
      length_in_columns - start_text_column;
  uint16_t available_columns_in_panel = marquee_width - start_display_column;
  if (available_columns_in_panel < number_of_text_columns_to_display) {
    number_of_text_columns_to_display = available_columns_in_panel;
  }

  marquee->place_string(
      &foreground,
      text,
      text_length,
      start_text_column,
      number_of_text_columns_to_display,
      0,
      start_display_column,
      type_face);

  marquee->show();
  return xQueuePeek(h_queue, &dummy_message, delay);
}

BaseType_t TextCrawlDriver::begin(
    const DisplayMessage& display_message,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  text = display_message.p_text;
  text_length = display_message.text_length;
  delay = display_message.delay1;
  foreground = display_message.foreground;
  background = display_message.background;
  length_in_columns = this->text_length * type_face.char_width();
  start_display_column = marquee->columns();
  start_text_column = 0;

  return advance_display(marquee, h_queue);
}

BaseType_t TextCrawlDriver::refresh(
    const DisplayMessage &display_message,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  return advance_display(marquee, h_queue);
}
