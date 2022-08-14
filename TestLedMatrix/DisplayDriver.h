/*
 * DisplayDriver.h
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * API for driving the display. Implementations support effects like
 *  * Clearing the screen
 *  * Display static (i.e. non-moving) text
 *  * Filling the display with a single color
 *
 *  Enhancement suggestion: display scrolling text
 *
 *  Display drivers are used as follows:
 *
 *  1.  The controlling task invokes begin() to set up the display and show the
 *      the original message.
 *
 *  2.  The controlling task repeatedly invokes refresh() to update the
 *      display.
 *
 * Please see the method documentation for details. Se also:
 *
 *   * DisplayMessage.h, the message providing display parameters.
 *
 *   * The FastLED library: https://fastled.io/docs/3.1/
 */

#ifndef DISPLAYDRIVER_H_
#define DISPLAYDRIVER_H_
#include<Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "FastLED.h"
#include "DisplayMessage.h"
#include "Marquee.h"

class DisplayDriver {
protected:

	static DisplayMessage display_message;  // Throw-away display message

public:
	DisplayDriver();
	virtual ~DisplayDriver();

	/**
	 * Starts the display, then waits on the specified queue with an
	 * implementation-dependent timeout.
	 *
	 * Parameters:
	 * ----------
	 *
	 *   Name              Contents
	 *   ----------------- ---------------------------------------------------
	 *   display_message   Configures the display, e.g. foreground and
	 *                     background colors. See DisplayhMessage.h for
	 *                     details.
	 *   marquee           The display
	 *   h_queue           The queue to watch.
	 *
	 * Returns: pdPASS if a message was found in the queue, errQUEUE_EMPTY if
	 *          the queue is empty.
	 */
	virtual BaseType_t begin(
		const DisplayMessage& display_message,
		Marquee *marquee,
		QueueHandle_t h_queue) = 0;

	/**
	 * Updates the display, then waits on the specified queue with an
	 * implementation-dependent timeout.
	 *
	 * Parameters:
	 * ----------
	 *
	 *   Name              Contents
	 *   ----------------- ---------------------------------------------------
	 *   display_message   Configures the display, e.g. foreground and
	 *                     background colors. See DisplayhMessage.h for
	 *                     details.
	 *   marquee           The display
	 *   h_queue           The queue to watch.
	 *
	 * Returns: pdPASS if a message was found in the queue, errQUEUE_EMPTY if
	 *          the queue is empty.
	 */
	virtual BaseType_t refresh(
		const DisplayMessage& display_message,
		Marquee *marquee,
		QueueHandle_t h_queue) = 0;
protected:
	DisplayMessage dummy_message;
};

#endif /* DISPLAYDRIVER_H_ */
