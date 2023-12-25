/*
 * SinglePixelLeftToRight.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 *
 * A DisplayDriver implementation that moves a single pixel across the
 * screen from left to right, and bottom to top, changing color after each
 * traversal. Note that the color order and delays are fixed, not taken
 * from the triggering DisplayMessage.
 */

#ifndef SINGLEPIXELLEFTTORIGHT_H_
#define SINGLEPIXELLEFTTORIGHT_H_

#include <Arduino.h>

#include "DisplayDriver.h"
#include "FastLED.h"

class SinglePixelLeftToRight: public DisplayDriver {
public:
  SinglePixelLeftToRight();

  virtual ~SinglePixelLeftToRight();

  virtual BaseType_t begin(
      const DisplayMessage &displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

  virtual BaseType_t refresh(const DisplayMessage &displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

private:

  BaseType_t write_pixel(
      Marquee *marquee,
      QueueHandle_t h_queue);
  int8_t color_index;
  CRGB current_color;
  int16_t row;
  int16_t column;
};

#endif /* SINGLEPIXELLEFTTORIGHT_H_ */
