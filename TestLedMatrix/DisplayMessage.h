/*
 * DisplayMessage.h
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * Message containing instructions to a display driver. The field names
 * suggest the field's use, but how values are used is completely up to
 * the receiving DisplayDriver implementation. Note that a display driver
 * can ignore any or all fields.
 */

#ifndef DISPLAYMESSAGE_H_
#define DISPLAYMESSAGE_H_

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "DisplayCommand.h"
#include "FastLED.h"

struct DisplayMessage {
  const unsigned char *p_text; // Text to display
  size_t text_length;          // Length of display text.
  DisplayCommand command;      // How to display the text.
  TickType_t delay1;           // First refresh delay
  TickType_t delay2;           // Second refresh delay
  CRGB foreground;             // Foreground color
  CRGB background;             // Background color

  DisplayMessage();
  const DisplayMessage& operator=(const DisplayMessage& rhs);
};

#endif /* DISPLAYMESSAGE_H_ */
