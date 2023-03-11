/*
 * NaturalOrderSinglePixelDriver.cpp
 *
 *  Created on: Jul 24, 2022
 *      Author: eric
 */

#include "NaturalOrderSinglePixelDriver.h"
#include "DisplayMessage.h"
#include "ColorTable.h"

static CRGB black = CRGB::Black;

NaturalOrderSinglePixelDriver::~NaturalOrderSinglePixelDriver() {
}

BaseType_t NaturalOrderSinglePixelDriver::begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  color_index = 0;
  pixel_index = 0;
  current_color = CRGB::Red;
  Serial.print("LED count: ");
  Serial.println(marquee->led_count());
  return set_led(marquee, h_queue);
}

BaseType_t NaturalOrderSinglePixelDriver::refresh(
  const DisplayMessage& displayMessage,
  Marquee *marquee,
  QueueHandle_t h_queue) {
  return set_led(marquee, h_queue);
}

BaseType_t NaturalOrderSinglePixelDriver::set_led(
    Marquee *marquee, QueueHandle_t h_queue) {
  const size_t led_count = marquee->led_count();

  marquee->flood(&black);
  marquee->set_pixel(pixel_index, &current_color);
  if (led_count <= ++pixel_index) {
    pixel_index = 0;
    color_index = (++color_index) % NUMBER_OF_CANONICAL_COLORS;
    current_color = canonical_colors[color_index];
  }
  marquee->show();

  return xQueuePeek(
    h_queue,
    &display_message,
    pdMS_TO_TICKS(20));
}
