/*
 * NaturalOrderSinglePixelDriver.h
 *
 *  Created on: Jul 24, 2022
 *      Author: Eric Mintz
 *
 * Display driver that cycles a single pixel through the canonical colors.
 * The illuminated pixel appears to move in natural order, i.e. backwards
 * within the LED array.  Note that the color order and delays are fixed,
 * not taken from the triggering DisplayMessage.
 */

#ifndef NATURALORDERSINGLEPIXELDRIVER_H_
#define NATURALORDERSINGLEPIXELDRIVER_H_

#include "DisplayDriver.h"
#include "FastLED.h"
#include "Marquee.h"

class NaturalOrderSinglePixelDriver : public DisplayDriver {
private:

	int8_t color_index;
	size_t pixel_index;
	CRGB current_color;

	BaseType_t set_led(Marquee *marquee, QueueHandle_t h_queue);

public:
	NaturalOrderSinglePixelDriver() :
		color_index(0),
		pixel_index(0),
		current_color(CRGB::Red) {
	}

	virtual ~NaturalOrderSinglePixelDriver();


	virtual BaseType_t begin(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue);

	virtual BaseType_t refresh(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue);
};

#endif /* NATURALORDERSINGLEPIXELDRIVER_H_ */
