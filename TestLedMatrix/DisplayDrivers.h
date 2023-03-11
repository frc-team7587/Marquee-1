/*
 * DisplayDrivers.h
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 */

#ifndef DISPLAYDRIVERS_H_
#define DISPLAYDRIVERS_H_
#include "DisplayDriver.h"
#include "DisplayCommand.h"

/**
 * A factory that produces DisplayDriver implementations. Please see
 * DisplayDrivers.cpp for details.
 */
class DisplayDrivers {

public:
  DisplayDrivers();
  ~DisplayDrivers();

  static DisplayDriver * of_type(DisplayCommand driverType);
};

#endif /* DISPLAYDRIVERS_H_ */
