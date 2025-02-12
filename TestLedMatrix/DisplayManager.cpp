/*
 * DisplayManager.cpp
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * Manages displays: subscribes to the command queue and activates display
 * driver, and runs it until commanded otherwise. Display drivers are
 * governed by command queue messages.
 */

#include "DisplayManager.h"

#include <stdlib.h>

#include "CommandIndicator.h"

void DisplayManager::begin(
    QueueHandle_t h_queue,
    QueueHandle_t h_command_indicator_queue) {
  CommandIndicator::CommandIndicatorMessage command_indicator_message;
  memset(&command_indicator_message, 0, sizeof(command_indicator_message));
  for (;;) {
    if (xQueueReceive(h_queue, &message, portMAX_DELAY) == pdPASS) {
      DisplayDriver *driver = DisplayDrivers::of_type(message.command);
      command_indicator_message.blink_count = message.command + 1;
      xQueueSendToBack(
          h_command_indicator_queue,
          &command_indicator_message,
          pdMS_TO_TICKS(1));
      Serial.println("Sent driver indicator message.");
      BaseType_t status = driver->begin(message, marquee, h_queue);
      while (status != pdPASS) {
        status = driver->refresh(message, marquee, h_queue);
      }
    }
  }
}
