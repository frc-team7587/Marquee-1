/*
 * DisplayFillDriver.cpp
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 */

#include "DisplayFillDriver.h"

DisplayFillDriver::DisplayFillDriver() {
}

DisplayFillDriver::~DisplayFillDriver() {
}

BaseType_t DisplayFillDriver::begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  marquee->flood(&displayMessage.foreground);
  marquee->show();
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));
}

BaseType_t DisplayFillDriver:: refresh(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(10000));
}
