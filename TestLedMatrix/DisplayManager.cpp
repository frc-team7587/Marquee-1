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
#include "DisplayDrivers.h"
#include "DisplayDriver.h"

void DisplayManager::begin(QueueHandle_t h_queue) {
	for (;;) {
		if (xQueueReceive(h_queue, &message, portMAX_DELAY) == pdPASS) {
			DisplayDriver *driver = drivers->of_type(message.command);
			BaseType_t status = driver->begin(message, marquee, h_queue);
			while (status != pdPASS) {
				status = driver->refresh(message, marquee, h_queue);
			}
		}
	}
}
