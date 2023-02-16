/*
 * FindSerialUsb.h
 *
 *  Created on: Aug 7, 2022
 *      Author: Eric Mintz
 *
 * Finds the first device whose path begins with /dev/ttyUSBn
 * or /dev/ttyACMn.
 */

#ifndef FINDSERIALUSB_H_
#define FINDSERIALUSB_H_
#include <string>

namespace application {

class FindSerialUsb {
	std::string device_path;
public:
	FindSerialUsb();
	~FindSerialUsb();
	std::string const device() {
		return device_path;
	}

};

} /* namespace application */

#endif /* FINDSERIALUSB_H_ */
