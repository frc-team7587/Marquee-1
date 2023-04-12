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

I2CCommandHandler::State I2CCommandHandler::TRANSITION_TABLE
    [I2CCommandHandler::I2C_NUMBER_OF_STATES]
    [I2CCommandHandler::I2C_NUMBER_OF_EVENTS] = {
  {  // I2C_HANDLER_STATE_CREATED
    I2C_HANDLER_STATE_ACCUMULATING,  // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_MESSAGE_ENDS
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_OVERFLOW, shouldn't happen
    I2C_HANDLER_STATE_UNDERFLOWED,  //  I2C_EVENT_UNDERFLOW
  },
  {  // I2C_HANDLER_STATE_ACCUMULATING
    I2C_HANDLER_STATE_ACCUMULATING,   // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_COMPLETE,  // I2C_EVENT_MESSAGE_ENDS
    I2C_HANDLER_STATE_OVERFLOWING,  // I2C_EVENT_OVERFLOW
    I2C_HANDLER_STATE_UNDERFLOWED,  // I2C_EVENT_UNDERFLOW
  },
  {  // I2C_HANDLER_STATE_COMPLETE
    I2C_HANDLER_STATE_ACCUMULATING,  // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_MESSAGE_ENDS
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_OVERFLOW
    I2C_HANDLER_STATE_UNDERFLOWED,  // I2C_EVENT_UNDERFLOW
  },
  {  // I2C_HANDLER_STATE_OVERFLOWING
    I2C_NUMBER_OF_STATES,   // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_OVERFLOWED,  // I2C_EVENT_MESSAGE_ENDS
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_OVERFLOW
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_UNDERFLOW
  },
  {  // I2C_HANDLER_STATE_OVERFLOWED
    I2C_HANDLER_STATE_ACCUMULATING,    // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_MESSAGE_ENDS
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_OVERFLOW
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_UNDERFLOW
  },
  {  // I2C_HANDLER_STATE_UNDERFLOWED
    I2C_HANDLER_STATE_ACCUMULATING,   // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_MESSAGE_ENDS
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_OVERFLOW
    I2C_HANDLER_STATE_ERROR,  // I2C_EVENT_UNDERFLOW
  },
  {  // I2C_HANDLER_STATE_ERROR
    I2C_NUMBER_OF_STATES,    // I2C_EVENT_CHARACTER
    I2C_HANDLER_STATE_ACCUMULATING,   // I2C_EVENT_MESSAGE_ENDS
    I2C_NUMBER_OF_STATES,    // I2C_EVENT_OVERFLOW, shouldn't happen
    I2C_NUMBER_OF_STATES,    // I2C_EVENT_UNDERFLOW
  },
};

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
  for (int i = 0; i < len; ++i) {
    uint8_t received_character = (uint8_t) Wire.read();
    Event event;
    if (received_character == 0xFF) {
      event = character_count < (MAX_MESSAGE_LEN - 1)
          ? I2C_EVENT_UNDERFLOW
          : I2C_EVENT_MESSAGE_ENDS;
    } else {
        event = character_count < (MAX_MESSAGE_LEN - 1)
            ? I2C_EVENT_CHARACTER
            : I2C_EVENT_OVERFLOW;
    }
    State previous_state = current_state;
    State maybe_new_state = TRANSITION_TABLE[current_state][event];
    if (maybe_new_state != I2C_NUMBER_OF_STATES) {
      switch (current_state = maybe_new_state) {
      case I2C_HANDLER_STATE_CREATED:
        // Nothing to do
        break;
      case I2C_HANDLER_STATE_ACCUMULATING:
        message_buffer[character_count++] = received_character;
        break;
      case I2C_HANDLER_STATE_COMPLETE:
        command_publisher->parse_and_publish(message_buffer);
        reset();
        break;
      case I2C_HANDLER_STATE_OVERFLOWING:
        // Wait for 0xFF.
        break;
      case I2C_HANDLER_STATE_OVERFLOWED:
        Serial.printf(
            "Message >>>%s<<< overflowed, discarding", message_buffer);
        break;
      case I2C_HANDLER_STATE_UNDERFLOWED:
        break;
      case I2C_HANDLER_STATE_ERROR:
        Serial.printf(
            "Illegal event %d received while in state %d.",
            event,
            previous_state);
        break;
      case I2C_NUMBER_OF_STATES:
        // Nothing to do
        break;
      }
    }
  }
}

void I2CCommandHandler::reset() {
  memset(message_buffer, 0, MESSAGE_BUFFER_LEN);
  character_count = 0;
}

I2CCommandHandler::I2CCommandHandler(
    CommandPublisher *command_publisher) :
    character_count(0),
    current_state(I2C_HANDLER_STATE_CREATED),
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
  Wire.setTimeout(500);
}
