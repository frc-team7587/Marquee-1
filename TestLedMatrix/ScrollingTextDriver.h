/*
 * SCrollingTextDriver.h
 *
 *  Created on: March 9, 2023
 *      Author: Richard Ho
 *
 * Displays scrolling (i.e. crawling) text on the panel. The incoming
 * DisplayMessage specifies the text, and foreground and background
 * colors.
 *
 * Delay 1 gives the delay after the initial frame, that is, the frame
 * containing the first part of the string. Delay 2 gives the delay
 * between subsequent frames. Note that the display restarts after the
 * last column leaves the LED array.
 *
 * ScrollingTextDriver.cpp provides the default display values to use
 * when the provided delay is 0.
 */

#include <Arduino.h>

#include "DisplayDriver.h"
#include "TypeFace.h"
#include "FastLED.h"

class ScrollingTextDriver : public DisplayDriver {
public:
  ScrollingTextDriver(const TypeFace& type_face);
  virtual ~ScrollingTextDriver();

  virtual BaseType_t begin(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

  virtual BaseType_t refresh(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

private:
  BaseType_t write_text(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

  const TypeFace& type_face;
  TickType_t initial_delay;
  TickType_t interframe_delay;
  int16_t column_shift;
};
