/*
 * StaticTextDriver.h
 *
 *  Created on: Jul 24, 2022
 *      Author: Eric Mintz
 *
 * Displays static (i.e. non-moving) text on the panel. The incoming
 * DisplayMessage specifies the text, and foreground and background
 * colors.
 */

#ifndef STATICTEXTDRIVER_H_
#define STATICTEXTDRIVER_H_

#include <Arduino.h>
#include "DisplayDriver.h"
#include "TypeFace.h"

class StaticTextDriver : public DisplayDriver {
public:
	StaticTextDriver(const TypeFace& type_face);
	virtual ~StaticTextDriver();


	virtual BaseType_t begin(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue);

	virtual BaseType_t refresh(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue);

private:
	const TypeFace& type_face;
};

#endif /* STATICTEXTDRIVER_H_ */
