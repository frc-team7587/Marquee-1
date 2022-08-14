/*
 * Marquee.cpp
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 */

#include "Marquee.h"


Marquee::~Marquee() {
}

void Marquee::set_pixel(int led_offset, const CRGB *color) {
	if (0 < led_offset && led_offset < num_leds) {
		leds[led_offset] = *color;
	}
}

void Marquee::flood(const CRGB *color) {
	for (int i = 0; i < num_leds; ++i) {
		leds[i] = *color;
	}
}

void Marquee::set_pixel(int row, int column, const CRGB *color) {
	int index = panel.index(row, column);
	set_pixel(index, color);
}

void Marquee::show() {
	vTaskDelay(pdMS_TO_TICKS(1));
	FastLED.show();
}
