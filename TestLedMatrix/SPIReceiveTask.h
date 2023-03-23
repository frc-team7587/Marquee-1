/*
 * SPIReceiver.h
 *
 *  Created on: Nov 26, 2022
 *      Author: Eric Mintz
 *
 * SPI Receive Task. The receiver runs open loop, meaning that there is no
 * handshake with the master. This means that the master must wait for this
 * receiver to ready itself sending. The sender:
 *
 * 1. Lowers chip select.
 * 2. Pauses to allow this slave to become ready. A one millisecond wait should
 *    be more than enough.
 * 3. Sends data, a text-formatted Marquee command
 * 4. Raises chip select
 *
 * The receiver then enqueues the transmitted data for processing.
 *
 * Note that the SPI pins are hard-wired into the receiver, so please create
 * only one instance. The class uses the VSPI, a.k.a. SPI3 SPI instances.
 *
 * GPIO Pin Assignments
 * ---- --- -----------
 *
 * +-----+--------------------------------------------------------------------+
 * | Pin | Function                                                           |
 * +-----+--------------------------------------------------------------------+
 * |  23 | VSPI Master Out/Slave In (MOSI) (default)                          |
 * +-----+--------------------------------------------------------------------+
 * |  19 | VSPI Master In/Slave Out (MISO) (default                           |
 * +-----+--------------------------------------------------------------------+
 * |  18 | VSPI Clock (SCK) (default)                                         |
 * +-----+--------------------------------------------------------------------+
 * |   5 | VSPI Slave Select (SS) (default)                                   |
 * +-----+--------------------------------------------------------------------+
 */

#ifndef SPIRECEIVETASK_H_
#define SPIRECEIVETASK_H_

#include "Arduino.h"

#include "driver/spi_slave.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "CommandPublisher.h"
#include "ReceivedCommand.h"

class SPIReceiveTask {
  size_t buffer_lengths;
  uint8_t *receive_buffer;
  uint8_t *send_buffer;
  spi_slave_transaction_t slave_transaction;
  ReceivedCommand received_command_message;
  CommandPublisher *command_publisher;
  TaskHandle_t h_task;

  /**
   * Runs the task.
   */
  void run();

  /**
   * Starts the receiver. Note that params must be a pointer to a
   * receive_configuration (see above) instance.
   */
  static void receive(void * params);

public:

  /**
   * Creates the receiver and allocates all required buffers..
   *
   * Parameters:
   * ----------
   *
   *   Name              Contents
   *   ----------------- ---------------------------------------------------
   *   command_publisher Parses and publishes the text formatted command
   *                     received from the RoboRio
   *   buffer_lengths    Length of the receive and send buffers in bytes
   *   receive_buffer    Receives data sent by the RoboRio. Must be word-aligned
   *                     and reside in DMA-accessible memory.
   *   send_buffer       Holds data to be sent to the RoboRio. Must be
   *                     word aligned and reside in DMA-accessible memory.
   */
  SPIReceiveTask(
    CommandPublisher *command_publisher,
    size_t buffer_lengths,
    uint8_t *receive_buffer,
    uint8_t *send_buffer);

  /**
   * Destructor -- for the sake of completeness. It's never invoked.
   */
  virtual ~SPIReceiveTask();

  /**
   * Configures the SPIO I/O pins
   */
  static void configure_gpio();

  /**
   * Starts the SPI receive task.
   */
  void start();
};

#endif /* SPIRECEIVETASK_H_ */
