/*
 * SPIReceiver.cpp
 *
 *  Created on: Nov 26, 2022
 *      Author: Eric Mintz
 */

#include "SPIReceiveTask.h"

#include <stdlib.h>

static spi_bus_config_t spi_bus_configuration = {
  .mosi_io_num=MOSI,
  .miso_io_num=MISO,
  .sclk_io_num=SCK,
  .quadwp_io_num = -1,
  .quadhd_io_num = -1,
};

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
  // TODO: error handling.
}

SPIReceiveTask::SPIReceiveTask(
    CommandPublisher *command_publisher,
    size_t buffer_lengths,
    uint8_t *receive_buffer,
    uint8_t *send_buffer) :
    command_publisher(command_publisher),
    buffer_lengths(buffer_lengths),
    receive_buffer(receive_buffer),
    send_buffer(send_buffer),
    h_task(NULL) {
  memset(&slave_transaction, 0, buffer_lengths);
  memset(&received_command_message, 0, buffer_lengths);
}

SPIReceiveTask::~SPIReceiveTask() {
}

void SPIReceiveTask::configure_gpio() {
  pinMode(MOSI, INPUT_PULLUP);
  pinMode(MISO, OUTPUT);
  pinMode(SCK, INPUT_PULLUP);
  pinMode(SS, INPUT_PULLUP);
}

void SPIReceiveTask::run() {
  set_spi_to_slave();
  for (;;) {
    memset(&slave_transaction, 0, sizeof(slave_transaction));
    memset(receive_buffer, 0, buffer_lengths);
    memset(send_buffer, 0, buffer_lengths);
    slave_transaction.length = buffer_lengths * 8;
    slave_transaction.rx_buffer = receive_buffer;
    slave_transaction.tx_buffer = send_buffer;
    esp_err_t status =
      spi_slave_transmit(SPI3_HOST, &slave_transaction, portMAX_DELAY);
    // TODO: error checking.
    command_publisher->parse_and_publish(receive_buffer);
  }
}

void SPIReceiveTask::receive(void * params) {
  ((SPIReceiveTask *) params)->run();
}

void SPIReceiveTask::start() {
  BaseType_t status = xTaskCreate(
      SPIReceiveTask::receive,
      "SPI receive",
      4096,
      this,
      4,
      &h_task);
  if (status == pdPASS) {
    Serial.println("SPI Read Task started.");
  }
}

