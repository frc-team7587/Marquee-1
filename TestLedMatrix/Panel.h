/*
 * panel.h
 *
 *  Created on: July 16, 2022
 *      Author: Eric Mintz
 *  Represents an LED array in the conventional coordinate system
 *
 * At the lowest (i.e. physical) level, displays are a linear array of pixels.
 * Implementations translate (row, column) pairs into array positions. In other
 * words, Panel implementations define a panel's underlying geometry.
 *
 *   ^
 *   |
 *   |
 *
 *   Y+
 *   ^
 * 7 |
 * 6 |
 * 5 |
 * 4 |
 * 3 |
 * 2 |
 * 1 |
 * 0 |
 *   +--------------------------------> X + -->
 *              1111111111222222222233
 *    01234567890123456789012345678901
 *
 * Note that a Panel represents a single physical panel, e.g. an 8 x 8, 8 x 32,
 * or other sized display. Panels can be daisy chained.
 */

#ifndef PANEL_H_
#define PANEL_H_

#include <Arduino.h>

class Panel {
public:
	/**
	 * Number of columns in the panel.
	 */
	virtual int columns() const = 0;

	/**
	 * Returns the number of LEDs in the panel.
	 */
	virtual int led_count() const = 0;

	/**
	 * Returns the offset of the specified row and column on the specified
	 * panel. This is used to locate an LED's position in the LED array.
	 * Panels are daisy chained left to right, with the leftmost panel
	 * being panel 0.
	 *
	 * Parameters:
	 * ----------
	 *
	 *   Name              Contents
	 *   ----------------- ---------------------------------------------------
	 *   row               Row number, with 0 being the bottom row
	 *   column            Column number, with 0 being the leftmost column
	 *   panel_no          Panel number, with 0 being the leftmost panel in
	 *                     the daisy chain.
	 *
	 */
	virtual int index(int row, int column, int panel_no) const = 0;

	/**
	 * Calculates the index of the LED at the specified row and column
	 * within a single panel. If panels are daisy chained, the returned
	 * index is relative to (0, 0) of the rightmost panel.
	 *
	 * Parameters:
	 * ----------
	 *
	 *   Name              Contents
	 *   ----------------- ---------------------------------------------------
	 *   row               Row number, with 0 being the bottom row
	 *   column            Column number, with 0 being the leftmost column
	 *
	 */
	int index(int row, int column) const {
		return index(row, column % columns(), column / columns());
	}

	/**
	 * Returns the number of rows in the panel.
	 */
	virtual int rows() const = 0;
protected:
	virtual ~Panel() {};
};

#endif /* PANEL_H_ */
