/*
 * DisplayTask.cpp
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "DisplayTask.h"
#include "DisplayMessage.h"
#include "FastLED.h"
#include "DisplayDrivers.h"


DisplayTask::DisplayTask(Marquee *marquee) :
  marquee(marquee) {
}

DisplayTask::~DisplayTask() {
}

void DisplayTask::run(QueueHandle_t h_queue) {
  Serial.println("Display manager task started.");
  // Start by blanking the display
  DisplayMessage message;
  message.p_text = 0;
  message.command = FILL_WITH_COLOR;
  message.delay1 = 0;
  message.delay2 = 0;
  message.foreground = CRGB::Black;
  message.background = CRGB::Black;
  for (;;) {
    DisplayDriver *driver = DisplayDrivers::of_type(message.command);
    BaseType_t status = driver->begin(message, marquee, h_queue);
    while (status == errQUEUE_EMPTY) {
      status = driver->refresh(message, marquee, h_queue);
    }
    xQueueReceive(h_queue, &message, pdMS_TO_TICKS(10));
    Serial.println("Display task received a message.");
  }
}
