/*
 * ErrorDisplayDriver.cpp
 *
 *  Created on: July 31, 2022
 *      Author: Eric Mintz
 */

#include "ErrorDisplayDriver.h"
#include "ColorTable.h"
#include "FastLED.h"

ErrorDisplayDriver::ErrorDisplayDriver() {
}

ErrorDisplayDriver::~ErrorDisplayDriver() {
}

BaseType_t ErrorDisplayDriver::begin(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue) {
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
	marquee->flood(color);
	marquee->show();
	return xQueuePeek(h_queue, &dummy_message, pdMS_TO_TICKS(250));
}
