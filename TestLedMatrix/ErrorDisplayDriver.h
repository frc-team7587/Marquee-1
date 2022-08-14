/*
 * ErrorDisplayDriver.h
 *
 *  Created on: July 31, 2022
 *      Author: Eric Mintz
 *
 * Displays an error panel, one that flashes with random colors.
 */

#ifndef ERRORDISPLAYDRIVER_H_
#define ERRORDISPLAYDRIVER_H_
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "DisplayMessage.h"
#include "Marquee.h"
#include "DisplayDriver.h"

class ErrorDisplayDriver : public DisplayDriver {
public:
	ErrorDisplayDriver();
	virtual ~ErrorDisplayDriver();

	virtual BaseType_t begin(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue);

	virtual BaseType_t refresh(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue);

private:
	BaseType_t fill_display(
			Marquee *marquee,
			QueueHandle_t h_queue);
};

#endif /* ERRORDISPLAYDRIVER_H_ */
