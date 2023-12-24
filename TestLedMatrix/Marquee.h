/*
 * Marquee.h
 *
 *  Created on: July 24, 2022
 *      Author: Eric Mintz
 *
 * Represents a marquee, which is a sequence of one or more panels.
 *
 * Enhancement suggestion: this class is incomplete, and has only been
 * used with a single physical panel. We will need to complete the class
 * if we want to support multiple daisy chained panels.
 *
 * See https://fastled.io/docs/3.1/
 *
 */

#ifndef MARQUEE_H_
#define MARQUEE_H_

#include <Arduino.h>
#include "FastLED.h"
#include "Panel.h"

class Marquee {

public:

  /**
   * Constructs a Marquee. Note that the Marquee integrates an LED array
   * with physical panels.
   *
   * Parameters:
   * ----------
   *
   *   Name              Contents
   *   ----------------- ---------------------------------------------------
   *   leds              An array of CRGB LED objects.
   *   num_leds          The length of the leds array
   *   panel             The Panel implementation that specifies the
   *                     physical display geometry.
   *
   */
  Marquee(CRGB *leds, size_t num_leds, const Panel& panel);

  /**
   * Sets a single array to a specified color.
   *
   * Parameters:
   * ----------
   *
   *   Name               ontents
   *   ----------------- ---------------------------------------------------
   *   row               The target pixel's row
   *   column            The target pixel's column
   *   color             The color to be set
   *
   */
  void set_pixel(int row, int column, const CRGB *color);

  /**
   * Returns the number of LEDs in a panel
   */
  size_t columns() {
    return panel.columns();
  }

  /**
   * Returns the number of LEDs in the display, in all daisy chained panels
   */
  size_t led_count() {
    return panel.led_count();
  }

  /**
   * Sets a ripple pattern beginning at the specified offset in the ripple
   * table. To avoid overflow, the offset is normalized (i.e. set to its value
   * modulo the ripple table length).
   *
   * Parameters:
   * ----------
   *
   *   Name    Contents
   *   ------- ---------------------------------------------------------------
   *   offset  The offset into the pattern that appears in display column
   *           0. Start at 0, please
   *
   * Returns: the offset to pass to the next invocation.
   */
  uint16_t ripple(uint16_t offset);

  /**
   * Returns the number of rows in the display
   */
  size_t rows() {
    return panel.rows();
  }

  /**
   * Sets a single pixel to a specified color
   *
   * Parameters:
   * ----------
   *
   *   Name              Contents
   *   ----------------- ---------------------------------------------------
   *   pixel_no          The pixel offset in the LED array
   *   color             The color to set
   *
   */
  void set_pixel(int pixel_no, const CRGB *color);

  /**
   * Sets all pixels in the display to a specified color.
   *
   * Parameters:
   * ----------
   *
   *   Name              Contents
   *   ----------------- ---------------------------------------------------
   *   color             The color to set, the flood color.
   *
   */
  void flood(const CRGB *color);

  /**
   * Sends the LED array to the panel, showing the configured pattern.
   */
  void show();

  virtual ~Marquee();

private:
  CRGB *leds;
  const size_t num_leds;
  const Panel& panel;
};

#endif /* MARQUEE_H_ */
