/*
 * ErrorDisplayDriver.cpp
 *
 *  Created on: July 31, 2022
 *      Author: Eric Mintz
 */

#include "ErrorDisplayDriver.h"
#include "ColorTable.h"
#include "FastLED.h"

static const CRGB medium_white = {
    .red = 63,
    .green = 63,
    .blue = 63,
};

static const CRGB black = CRGB::Black;

static const unsigned char what_happened[] = {0x81, 0x81, 0x81, 0x81, 0x81};

ErrorDisplayDriver::ErrorDisplayDriver(TypeFace& type_face) :
    type_face(type_face),
    rainbow_offset(0) {
}

ErrorDisplayDriver::~ErrorDisplayDriver() {
}

BaseType_t ErrorDisplayDriver::begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  rainbow_offset = 0;
  return fill_display(marquee, h_queue);
}

BaseType_t ErrorDisplayDriver::refresh(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  return fill_display(marquee, h_queue);
}

BaseType_t ErrorDisplayDriver::fill_display(
    Marquee *marquee,
    QueueHandle_t h_queue) {
  const CRGB* color =
      canonical_colors + (esp_random() % NUMBER_OF_CANONICAL_COLORS);
  rainbow_offset = marquee->ripple(rainbow_offset);
  marquee->place_string(
      &black,
      reinterpret_cast<const unsigned char *>(what_happened),
      5,
      0,
      5 * type_face.char_width(),
      0,
      1,
      type_face);
  marquee->show();
  return xQueuePeek(h_queue, &dummy_message, pdMS_TO_TICKS(250));
}
