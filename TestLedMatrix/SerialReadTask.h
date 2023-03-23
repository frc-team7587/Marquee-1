/*
 * SerialReadTask.h
 *
 *  Created on: July 30, 2022
 *      Author: Eric Mintz
 *
 * Reads messages from the serial (i.e. USB) port, converts them into
 * Display Messages, and enqueues them for processing.  The message format is
 * <text>|<command_number>|<command>|<delay_1>|<delay_2>|
 * <foreground_red>|<foreground_green>|<foreground_blue>|
 * <background_red>|<background_green>|<background_blue>
 * '\r''\n'
 *
 * Note that:
 *
 *   The message is limited to 124 characters, all in.
 *   All numbers are decimal
 *   Delays are in milliseconds between 0 and 32767 inclusive
 *   The display text can be empty and must not contain '|'.  Only characters
 *   between ' ' and '~' (inclusive) EXCEPT '|' are supported.
 *   The command number is the index of the corresponding command enumeration *
 *   Colors are between 0 and 255 inclusive.
 *   The '\r' character is ignored and optional
 *   The design is optimized for 115200 baud or 11520 characters per second or
 *   11.52 characters per millisecond. It is recommended that the serial buffer
 *   be increased to 128 or more bytes to guarantee successful operation.
 */

#ifndef SERIALREADTASK_H_
#define SERIALREADTASK_H_

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "CommandPublisher.h"

#define INPUT_BUFFER_SIZE 128

class SerialReadTask {
  enum SerialReadStatus {
    READING,
    OVERRUN,
  };

  CommandPublisher *command_publisher;
  unsigned char input_buffer[INPUT_BUFFER_SIZE];
public:
  SerialReadTask(CommandPublisher *command_publisher);
  virtual ~SerialReadTask();

  /**
   * Infinite loop that reads from the serial USB connection, decodes the
   * messages, forwards them to the command publisher that was bound during
   * construction.
   */
  void run();

private:
  unsigned char command_buffer[256];
};


#endif /* SERIALREADTASK_H_ */
