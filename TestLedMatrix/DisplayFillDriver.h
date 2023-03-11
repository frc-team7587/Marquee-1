/*
 * DisplayFillDriver.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 *
 * DisplayDriver implementation that fills the entire panel with a singoel
 * color.
 */

#ifndef DISPLAYFILLDRIVER_H_
#define DISPLAYFILLDRIVER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "DisplayDriver.h"

class DisplayFillDriver : public DisplayDriver {
public:
  DisplayFillDriver();
  virtual ~DisplayFillDriver();

  virtual BaseType_t begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue);

  virtual BaseType_t refresh(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue);
};

#endif /* DISPLAYFILLDRIVER_H_ */
