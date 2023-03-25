/*
 * I2CCommandHandler.cpp
 *
 *  Created on: Mar 24, 2023
 *      Author: Eric Mintz
 */

#include "I2CCommandHandler.h"

#include "freertos/FreeRTOS.h"

#include "Wire.h"

I2CCommandHandler * I2CCommandHandler::command_handler = NULL;

// The slave address. Note that the receiver uses a 7 but slave address,
// so the address must belong to [0 .. 0x7F].
#define SLAVE_ADDRESS 0xA

void I2CCommandHandler::onReceive(int len) {
  Serial.print("I2CCommandHandler::onReceive invoked, character count: ");
  Serial.println(len);
  command_handler->receive_and_forward(len);
}

void I2CCommandHandler::onRequest() {
}

void I2CCommandHandler::receive_and_forward(int len) {
  if (len > MAX_MESSAGE_LEN) {
    Serial.print("The message length, ");
    Serial.print(len);
    Serial.print(" is longer than the allowed maximum of ");
    Serial.println(MAX_MESSAGE_LEN);
    len = MAX_MESSAGE_LEN;
  } else {
    Serial.print(len);
    Serial.println(" bytes received from I2C master.");
  }

  uint8_t *destination = message_buffer;
  for (int i = 0; i < len; ++i) {
    *destination++ = (uint8_t) Wire.read();
  }
  Serial.print("I2C received >>>");
  Serial.print((char *) message_buffer);
  Serial.println("<<<");
  command_publisher->parse_and_publish(message_buffer);
  Wire.flush();
  reset();
}

void I2CCommandHandler::reset() {
  memset(message_buffer, 0, MESSAGE_BUFFER_LEN);
  character_count = 0;
}

I2CCommandHandler::I2CCommandHandler(
    CommandPublisher *command_publisher) :
    command_publisher(command_publisher) {
  reset();
}

I2CCommandHandler::~I2CCommandHandler() {
}

void I2CCommandHandler::begin() {
  command_handler = this;
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin(SLAVE_ADDRESS);
  Wire.setBufferSize(MESSAGE_BUFFER_LEN);
}
