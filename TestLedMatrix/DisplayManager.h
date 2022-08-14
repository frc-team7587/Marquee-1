/*
 * DisplayManager.h
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * Manages a display, including polling and refresh.
 */

#ifndef DISPLAYMANAGER_H_
#define DISPLAYMANAGER_H_

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "DisplayDrivers.h"
#include "DisplayMessage.h"
#include "Marquee.h"
#include "Panel.h"

class DisplayManager {

	DisplayDrivers *drivers;
	DisplayMessage message;
	Marquee *marquee;

public:

	DisplayManager(DisplayDrivers *drivers, Marquee *marquee) {
		this->drivers = drivers;
		this->marquee = marquee;
	}

	void begin(QueueHandle_t queue_handle);
};

#endif /* DISPLAYMANAGER_H_ */
