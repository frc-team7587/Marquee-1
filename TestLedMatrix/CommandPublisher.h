/*
 * CommandPublisher.h
 *
 *  Created on: Jan 31, 2023
 *      Author: Metuchen Momentum, FRC Robotics Team 7587
 *
 * Parses text input into a marquee command and publishes the latter on
 * the command queue.
 */

#ifndef COMMANDPUBLISHER_H_
#define COMMANDPUBLISHER_H_

#include "Arduino.h"

#include "stdlib.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "DisplayMessage.h"

#define MAX_TEXT_LENGTH 128

class CommandPublisher {
public:
	/**
	 * States for the DFA that decodes incoming command strings. See
	 * https://en.wikipedia.org/wiki/Deterministic_finite_automaton
	 * for an explanation of DFA.
	 */
	enum STATE {
		TEXT,
		TEXT_END,
		COMMAND,
		COMMAND_END,
		DELAY_1,
		DELAY_1_END,
		DELAY_2,
		DELAY_2_END,
		FOREGROUND_RED,
		FOREGROUND_RED_END,
		FOREGROUND_GREEN,
		FOREGROUND_GREEN_END,
		FOREGROUND_BLUE,
		FOREGROUND_BLUE_END,
		BACKGROUND_RED,
		BACKGROUND_RED_END,
		BACKGROUND_GREEN,
		BACKGROUND_GREEN_END,
		BACKGROUND_BLUE,
		IGNORING_RETURN,
		END_OF_COMMAND,
		ERROR,
		END_OF_ERROR,
		NUMBER_OF_STATES,  // MUST be last
	};

private:
	QueueHandle_t h_command_queue;
	DisplayMessage command_to_publish;
	unsigned char command_buffer[MAX_TEXT_LENGTH];

	void init();

public:

	CommandPublisher();
	virtual ~CommandPublisher();

	/**
	 * Binds this CommandPublisher to the specified FreeRTOS queue, which makes
	 * the specified queue the publication target.
	 *
	 * Parameters:
	 * ----------
	 *
	 *   Name              Contents
	 *   ----------------- ---------------------------------------------------
	 *   h_command_queue   Handle of the FreeRTOS Queue that transmits
	 *                     DisplayMessage instances from this task to the
	 *                     display management task.
	 *
	 *
	 */
	void begin(QueueHandle_t h_command_queue);

	/**
	 * Parse a raw, text command into a CommandMessage and publish the latter.
	 *
	 * Parameters:
	 * ----------
	 *
	 *   Name              Contents
	 *   ----------------- ---------------------------------------------------
	 *   raw_command       Text command as sent by the RoboRio.
	 */
	void parse_and_publish(const unsigned char * raw_command);

	void publish_error();
};

#endif /* COMMANDPUBLISHER_H_ */
