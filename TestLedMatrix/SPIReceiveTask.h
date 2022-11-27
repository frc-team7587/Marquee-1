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
 * 3. Sends data
 * 4. Raises chip select
 *
 * The receiver then enqueues the transmitted data for processing, and
 * must carry ReceivedCommand instances.
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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/spi_slave.h"
#include "ReceivedCommand.h"

class SPIReceiveTask {
	uint8_t *receive_buffer;
	uint8_t *send_buffer;
	spi_slave_transaction_t slave_transaction;
	ReceivedCommand received_command_message;

public:

	/**
	 * Parameter passed to the receive task startup.
	 */
	typedef struct {
		// Forward all input to the following queue.
		QueueHandle_t h_command;

		// The receive task to start
		SPIReceiveTask *receiver;
	} receive_configuration;

	/**
	 * Creates the receiver and allocates all required buffers..
	 */
	SPIReceiveTask();

	/**
	 * Destructor -- for the sake of completeness. It's never invoked.
	 */
	virtual ~SPIReceiveTask();

	/**
	 * Runs the task.
	 */
	void run(receive_configuration *config);

	/**
	 * Starts the receiver. Note that params must be a pointer to a
	 * receive_configuration (see above) instance.
	 */
	static void receive(void * params);
};

#endif /* SPIRECEIVETASK_H_ */
