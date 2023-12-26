/*
 * DisplayDrivers.cpp
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 *
 * A DisplayDriver factory that returns the factory type requested. To support
 * a new display driver:
 *
 * 1.  Implement the driver. Note that the new driver must subclass
 *     DisplayDriver.
 * 2.  Add a new value to the DisplayCommand enumeration.
 *
 * 3.  Include the new driver's header below.
 *
 * 4.  Create a static instance of the driver along with the existing
 *     instances below.
 *
 * 5.  Add a case that returns a pointer to the new driver.
 *
 * Suggestions for future enhancement: replace the switch statement with
 * an std::map.
 */

#include "DisplayDrivers.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


#include "NaturalOrderSinglePixelDriver.h"
#include "DisplayCommand.h"
#include "DisplayFillDriver.h"
#include "ErrorDisplayDriver.h"
#include "RippleDriver.h"
#include "StaticTextDriver.h"
#include "SinglePixelLeftToRight.h"
#include "ScrollingTextDriver.h"
#include "TypeFace6x8.h"

TypeFace6x8 type_face;


static DisplayFillDriver display_fill;
static ErrorDisplayDriver error_display;
static NaturalOrderSinglePixelDriver natural_order_single_pixel;
static RippleDriver ripple_driver;
static StaticTextDriver static_text(type_face);
static SinglePixelLeftToRight single_pixel_left_to_right;
static ScrollingTextDriver scrolling_text(type_face);


DisplayDrivers::DisplayDrivers() {
}

DisplayDrivers::~DisplayDrivers() {
};

DisplayDriver * DisplayDrivers::of_type(DisplayCommand driver_type) {
  DisplayDriver *result = NULL;
  int driver_command = driver_type;
  Serial.print("Display command: ");
  Serial.println(driver_command);
  switch(driver_type) {
    default:
    case ERROR:
      result = &ripple_driver;
      break;
    case FILL_WITH_COLOR:
    case FLASH_TWO_COLORS:  // TODO
      result = &display_fill;
      break;
    case RIPPLING_RAINBOW:
      result = &ripple_driver;
      break;
    case SINGLE_PIXEL_NATURAL_ORDER:
      result = &natural_order_single_pixel;
      break;
    case SINGLE_PIXEL_LEFT_TO_RIGHT:
      result = &single_pixel_left_to_right;
      break;
    case STATIC_TEXT:
      result = &static_text;
      break;
    case SCROLLING_TEXT:
        result = &scrolling_text;
        break;
  }

  return result;
}

// TODO(emintz): use a map.
