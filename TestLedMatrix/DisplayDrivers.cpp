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
#include "TypeFace6x8.h"
#include "StaticTextDriver.h"
#include "SinglePixelLeftToRight.h"

TypeFace6x8 type_face;


DisplayFillDriver display_fill;
ErrorDisplayDriver error_display;
NaturalOrderSinglePixelDriver natural_order_single_pixel;
StaticTextDriver static_text(type_face);
SinglePixelLeftToRight single_pixel_left_to_right;


DisplayDrivers::DisplayDrivers() {
}

DisplayDrivers::~DisplayDrivers() {
};

DisplayDriver * DisplayDrivers::of_type(DisplayCommand driver_type) {
  DisplayDriver *result = 0;
  switch(driver_type) {
    default:
    case ERROR:
      result = &error_display;
      break;
    case FLASH_TWO_COLORS:  // TODO
    case FILL_WITH_COLOR:
      result = &display_fill;
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
  }

  return result;
}

// TODO(emintz): use a map.
