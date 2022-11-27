/*
 * SPIReceiver.cpp
 *
 *  Created on: Nov 26, 2022
 *      Author: Eric Mintz
 */

#include "SPIReceiveTask.h"

#include <stdlib.h>
#include "esp_heap_caps.h"

static spi_bus_config_t spi_bus_configuration = {
	.mosi_io_num=MOSI,
	.miso_io_num=MISO,
	.sclk_io_num=SCK,
	.quadwp_io_num = -1,
	.quadhd_io_num = -1,};

static spi_slave_interface_config_t slave_interface_configuration = {
	.spics_io_num = SS,
	.flags=0,
	.queue_size=3,
	.mode = SPI_MODE0,
	.post_setup_cb=NULL,
	.post_trans_cb=NULL,
};

void set_spi_to_slave() {
	esp_err_t status = spi_slave_initialize(
		SPI3_HOST,
		&spi_bus_configuration,
		&slave_interface_configuration,
		SPI_DMA_CH_AUTO);
	if (status != ESP_OK) {
		// TODO error checking
	}
}

SPIReceiveTask::SPIReceiveTask() {
	receive_buffer =
		(uint8_t *) heap_caps_malloc(MAX_COMMAND_LENGTH, MALLOC_CAP_DMA);
	send_buffer =
		(uint8_t *) heap_caps_malloc(MAX_COMMAND_LENGTH, MALLOC_CAP_DMA);
	memset(&slave_transaction, 0, sizeof(slave_transaction));
	memset(&received_command_message, 0, sizeof(received_command_message));
}

SPIReceiveTask::~SPIReceiveTask() {
}

void SPIReceiveTask::run(receive_configuration *config) {
	set_spi_to_slave();
	for (;;) {
		memset(&slave_transaction, 0, sizeof(slave_transaction));
		memset(receive_buffer, 0, MAX_COMMAND_LENGTH);
		memset(send_buffer, 0, MAX_COMMAND_LENGTH);
		slave_transaction.length = MAX_COMMAND_LENGTH * 8;
		slave_transaction.rx_buffer = receive_buffer;
		slave_transaction.tx_buffer = send_buffer;
		esp_err_t status =
			spi_slave_transmit(SPI3_HOST, &slave_transaction, portMAX_DELAY);
		// TODO: error checking.
		memmove(
			received_command_message.command,
			receive_buffer,
			sizeof(received_command_message.command));
		if (xQueueSendToBack(
			config->h_command,
			(void *) &received_command_message,
			pdMS_TO_TICKS(100)) != pdPASS) {
			// TODO error checking.
		}
	}
}

void SPIReceiveTask::receive(void * params) {
	receive_configuration *config = (receive_configuration *) params;
	config->receiver->run(config);
}

