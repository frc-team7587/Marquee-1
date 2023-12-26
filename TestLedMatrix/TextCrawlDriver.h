/*
 * TextCrawlDriver.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Eric Mintz
 *
 * Scroll text smoothly across the display. Scroll the text one column to the
 * right at each step. Start with a single column at the extreme left, fill
 * the screen in to the right, then push the text till the screen goes
 * blank.
 */

#ifndef TEXTCRAWLDRIVER_H_
#define TEXTCRAWLDRIVER_H_

#include "Arduino.h"
#include "DisplayDriver.h"
#include "DisplayCommand.h"
#include "TypeFace.h"

class TextCrawlDriver : public DisplayDriver {

  const TypeFace& type_face;
  const unsigned char * text;
  size_t text_length;
  TickType_t delay;
  CRGB foreground;
  CRGB background;
  uint16_t length_in_columns;

  uint16_t start_display_column;  // Starting column on the display
  uint16_t start_text_column;     // First column  of text pixels to display

  DisplayMessage dummy_message;

  BaseType_t advance_display(
      Marquee *marquee,
      QueueHandle_t h_queue);

public:
  TextCrawlDriver(const TypeFace &type_face);
  virtual ~TextCrawlDriver();

  virtual BaseType_t begin(
    const DisplayMessage& display_message,
    Marquee *marquee,
    QueueHandle_t h_queue);

  virtual BaseType_t refresh(
    const DisplayMessage& display_message,
    Marquee *marquee,
    QueueHandle_t h_queue);

};

#endif /* TEXTCRAWLDRIVER_H_ */
