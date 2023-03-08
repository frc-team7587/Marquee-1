/*
 * CommandIndicator.cpp
 *
 *  Created on: Feb 16, 2023
 *      Author: Eric Mintz
 */

#include "CommandIndicator.h"

#include "Arduino.h"

#include <stdlib.h>
#include <string.h>

#define BUILTIN_LED 2

CommandIndicator::CommandIndicator() :
  h_indicator_queue(NULL),
  blinks_per_iteration(0) {

}

CommandIndicator::~CommandIndicator() {
}

void CommandIndicator::run_loop(void) {
  CommandIndicatorMessage message;
  memset(&message, 0, sizeof(message));
  digitalWrite(BUILTIN_LED, LOW);
  for (;;) {
    while (xQueueReceive(h_indicator_queue, &message, 0) == pdTRUE) {
      blinks_per_iteration = message.blink_count;
    }
    for (uint16_t i = 0; i < blinks_per_iteration; ++i) {
      digitalWrite(BUILTIN_LED, HIGH);
      vTaskDelay(pdMS_TO_TICKS(150));
      digitalWrite(BUILTIN_LED, LOW);
      vTaskDelay(pdMS_TO_TICKS(150));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

TaskHandle_t CommandIndicator::start(QueueHandle_t h_indicator_queue) {
  this->h_indicator_queue = h_indicator_queue;
  TaskHandle_t task_handle = NULL;

  xTaskCreate(
      start_run_loop,
      "Indicator",
      2048,
      this,
      1,
      &task_handle);

  return task_handle;
}

void CommandIndicator::start_run_loop(void *params) {
  ((CommandIndicator *) params)->run_loop();
}
