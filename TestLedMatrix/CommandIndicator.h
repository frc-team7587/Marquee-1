/*
 * CommandIndicator.h
 *
 *  Created on: Feb 16, 2023
 *      Author: Eric Mintz
 *
 * Blinks the built-in LED to reflect the command currently being run.
 */

#ifndef COMMANDINDICATOR_H_
#define COMMANDINDICATOR_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

class CommandIndicator {
  QueueHandle_t h_indicator_queue;  // Carries incoming commands
  uint16_t blinks_per_iteration;  // Number of times to blink in each iteration

  /**
   * The main event loop, which blinks the built-in LED.
   */
  void run_loop();

  /**
   * The task start function, which invokes the run loop.
   *
   * Parameter      Contents
   * -------------- -------------------------------------------------------
   * params         Parameter, the CommandIndicator instance to run
   */
  static void start_run_loop(void *params);

public:
  struct CommandIndicatorMessage {
    uint16_t blink_count;
  } ;

  CommandIndicator();
  virtual ~CommandIndicator();

  /**
   * Starts the run loop.
   *
   * Parameter         Contents
   * --------------    -------------------------------------------------------
   * h_indicator_queue The queue that provides commands.
   */
  TaskHandle_t start(QueueHandle_t h_indicator_queue);
};

#endif /* COMMANDINDICATOR_H_ */
