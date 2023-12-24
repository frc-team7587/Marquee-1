/*
 * RippleDriver.cpp
 *
 *  Created on: Dec 21, 2023
 *      Author: Eric Mintz
 */

#include "RippleDriver.h"

RippleDriver::RippleDriver() :
  offset(0) {
}

RippleDriver::~RippleDriver() {
}

BaseType_t RippleDriver::fill(
  const DisplayMessage& displayMessage,
  Marquee *marquee,
  QueueHandle_t h_queue) {
    offset = marquee->ripple(offset);
    marquee->show();
  return xQueuePeek(h_queue, &display_message, displayMessage.delay1);
}

BaseType_t RippleDriver::begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  offset = 0;
  return fill(displayMessage, marquee, h_queue);
}

BaseType_t RippleDriver::refresh(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  return fill(displayMessage, marquee, h_queue);
}
