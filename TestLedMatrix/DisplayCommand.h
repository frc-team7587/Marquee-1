/*
 * DisplayCommand.h
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * How to display text, the display format.
 *
 */

#ifndef DISPLAYCOMMAND_H_
#define DISPLAYCOMMAND_H_

enum DisplayCommand {
	ERROR,
	FILL_WITH_COLOR,
	FLASH_TWO_COLORS,
	SINGLE_PIXEL_NATURAL_ORDER,
	SINGLE_PIXEL_LEFT_TO_RIGHT,
	STATIC_TEXT,
    SCROLLING_TEXT,
	NUMBER_OF_COMMANDS,   // MUST BE LAST!
};

#endif /* DISPLAYCOMMAND_H_ */
