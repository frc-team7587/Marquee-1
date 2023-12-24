/*
 * RippleDriver.h
 *
 *  Created on: Dec 21, 2023
 *      Author: Eric Mintz
 *
 * Display a rainbow ripple pattern on the marquee
 */

#ifndef RIPPLEDRIVER_H_
#define RIPPLEDRIVER_H_

#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "DisplayDriver.h"

class RippleDriver : public DisplayDriver {
  uint16_t offset;

  BaseType_t fill(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t queue);

public:
  RippleDriver();
  virtual ~RippleDriver();

  virtual BaseType_t begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue);

  virtual BaseType_t refresh(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue);
};

#endif /* RIPPLEDRIVER_H_ */
