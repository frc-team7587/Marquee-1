/*
 * I2CCommandHandler.h
 *
 * Receives Marquee commands over I2C and forwards them for processing
 *
 *  Created on: Mar 24, 2023
 *      Author: Eric Mintz
 *
 * Note: The I2C Command Handler uses static callbacks to process incoming
 *       commands. Create one and only one instance (not that there's any
 *       reason to do so). Creating multiple instances will cause undefined
 *       and almost certainly undesirable behavior.
 *
 *  Note: the slave address is hard-wired. See I2CCommandHandler.cpp for
 *        details.
 */

#ifndef I2CCOMMANDHANDLER_H_
#define I2CCOMMANDHANDLER_H_

#include "Arduino.h"

#include "CommandPublisher.h"

#define MAX_MESSAGE_LEN 128
#define MESSAGE_BUFFER_LEN (MAX_MESSAGE_LEN + 2)

class I2CCommandHandler {
  enum State {
    I2C_HANDLER_STATE_CREATED,  // Initial state
    I2C_HANDLER_STATE_ACCUMULATING,  // Accumulating characters
    I2C_HANDLER_STATE_COMPLETE,  // Message completed normally (no errors).
    I2C_HANDLER_STATE_OVERFLOWING,  // Message overflowing its buffer
    I2C_HANDLER_STATE_OVERFLOWED,  // Overflowing message ended
    I2C_HANDLER_STATE_UNDERFLOWED,  // Message too short
    I2C_HANDLER_STATE_ERROR,  // An internal error occurred.
    I2C_NUMBER_OF_STATES,  // State count, MUST be last
  };

  enum Event {
    I2C_EVENT_CHARACTER,  // Message character received
    I2C_EVENT_MESSAGE_ENDS,  // Message end (0xFF) received when expected
    I2C_EVENT_OVERFLOW,  // Excess character received
    I2C_EVENT_UNDERFLOW,  // Message end (0xFF) received early.
    I2C_NUMBER_OF_EVENTS,  // Event count, MUST be last.
  };

  static State TRANSITION_TABLE[I2C_NUMBER_OF_STATES][I2C_NUMBER_OF_EVENTS];

  static I2CCommandHandler * command_handler;

  uint8_t message_buffer[MESSAGE_BUFFER_LEN];
  size_t character_count;
  State current_state;

  CommandPublisher *command_publisher;

  /**
   * Invoked when the master requests data. This should not happen, so the
   * method does nothing.
   */
  static void onRequest();

  /**
   * Invoked when the master sends data. The function passes the request to
   * I2CCommandHandler::receive_and_forward.
   */
  static void onReceive(int len);

  /**
   * Receives a command from the master and forwards it to the associated
   * CommandPublisher, which forwards it to the Marquee handler.
   */
  void receive_and_forward(int len);

  /**
   * Clears the buffer and sets the character count to 0.
   */
  void reset();

public:
  /**
   * Constructor
   *
   * Parameters
   *
   * Name                   Contents
   * ---------------------- --------------------------------------------
   * command_publisher      CommandProcessor which parses the incoming
   *                        command and forwards the result to the
   *                        Marquee Controller
   */
  I2CCommandHandler(CommandPublisher *command_publisher);
  virtual ~I2CCommandHandler();

  /**
   * Register the callbacks and starts processing incoming messages
   */
  void begin();
};

#endif /* I2CCOMMANDHANDLER_H_ */
