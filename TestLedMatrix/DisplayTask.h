/*
 * DisplayTask.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric MNintz
 *
 * FreeRTOS Task that runs the display.
 */

#ifndef DISPLAYTASK_H_
#define DISPLAYTASK_H_

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Marquee.h"

class DisplayTask {
public:
  DisplayTask(Marquee *marquee);
  virtual ~DisplayTask();

  /**
   * The task, an endless loop that manages the display
   *
   * Parameters:
   * ----------
   *
   *   Name              Contents
   *   ----------------- ---------------------------------------------------
   *   h_queue           Handle to the FreeRTOS queue that carries display
   *                     messages to the display management loop.
   */
  void run(QueueHandle_t h_queue);

private:
  Marquee *marquee;
};

#endif /* DISPLAYTASK_H_ */
