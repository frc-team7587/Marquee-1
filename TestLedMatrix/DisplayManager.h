/*
 * DisplayManager.h
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * Manages a display, including polling and refresh.
 */

#ifndef DISPLAYMANAGER_H_
#define DISPLAYMANAGER_H_

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "DisplayDrivers.h"
#include "DisplayMessage.h"
#include "Marquee.h"
#include "Panel.h"

class DisplayManager {

  DisplayMessage message;
  Marquee *marquee;

public:

  /**
   * Constructor -- creates DisplayManager instances
   *
   * Parameter              Contents
   * ---------------------- -----------------------------------------------
   * drivers                Display driver factory that provides low-level
   *                        marquee drive3rs to the display manager
   * marquee                The marquee that displays the text
   */
  DisplayManager(Marquee *marquee) :
    marquee(marquee) {
  }

  /**
   * Starts the display manager
   *
   * Parameter               Contents
   * ----------------------  -----------------------------------------------
   * queue_handle            Handle to the queue that provides display
   *                         commands
   * command_indicator_queue The display driver sends indicator commands via
   *                         this queue. The receiver blinks the built-in LED
   *                         as directed by the command.
   */
  void begin(
      QueueHandle_t queue_handle,
      QueueHandle_t command_indicator_queue);
};

#endif /* DISPLAYMANAGER_H_ */
