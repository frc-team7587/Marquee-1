/*
 * SerialReadTask.cpp
 *
 *  Created on: July 30, 2022
 *      Author: Eric Mintz
 *
 * Reads a text-formatted Marquee command from the USB serial input and
 * forwards to a command publisher for transmission to the display
 * controller. Note that this is used only for debugging. The RoboRio
 * will communicate with the Marquee via SPI.
 */
#include "Arduino.h"
#include <stdlib.h>

#include "SerialReadTask.h"
#include "DisplayCommand.h"
#include "DisplayMessage.h"

SerialReadTask::SerialReadTask(CommandPublisher *command_publisher) :
	command_publisher(command_publisher) {
	memset(input_buffer, 0, sizeof(input_buffer));
}

SerialReadTask::~SerialReadTask() {
}

void SerialReadTask::run() {
	Serial.println("Serial read task started.");
	SerialReadStatus status = READING;
	memset(input_buffer, 0, sizeof(input_buffer));
	int characters_read = 0;
	for (;;) {
		while (Serial.available()) {
			int incoming_byte = Serial.read();
			switch (status) {
			case READING:
				if (characters_read < sizeof(input_buffer) - 1) {
					input_buffer[characters_read++] = (unsigned char) incoming_byte;
					if (incoming_byte == '\n') {
						command_publisher->parse_and_publish(input_buffer);
						memset(input_buffer, 0, sizeof(input_buffer));
						characters_read = 0;
					}
				} else {
					Serial.println("Overrun.");
					status = OVERRUN;
				}
				break;

			case OVERRUN:
				if (incoming_byte == '\n') {
					command_publisher->publish_error();
					characters_read = 0;
					memset(input_buffer, 0, sizeof(input_buffer));
					status = READING;
					Serial.println("Overrun ended.");
				}
				break;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
