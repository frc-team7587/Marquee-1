/*
 * FindSerialUsb.cpp
 *
 *  Created on: Aug 7, 2022
 *      Author: Eric Mintz
 */

#include "FindSerialUsb.h"
#include <dirent.h>
#include <iostream>
#include <string>

#define DEVICE_DIRECTORY "/dev/"
#define TTY_USB "ttyUSB"
#define TTY_ACM "ttyACM"

namespace application {

FindSerialUsb::FindSerialUsb() :
	device_path("") {
	DIR *dir = opendir(DEVICE_DIRECTORY);
	std::string device_directory(DEVICE_DIRECTORY);
	if (dir) {
		std::string tty_usb(TTY_USB);
		std::string tty_acm(TTY_ACM);
		struct dirent *ent;
		while((ent = readdir(dir))) {
			std::string serial_device(ent->d_name);
			if (!serial_device.compare(0, tty_usb.length(), tty_usb)
				|| !serial_device.compare(0, tty_acm.length(), tty_acm)) {
				device_path = std::string(DEVICE_DIRECTORY) + serial_device;
				break;
			}
		}
	}

	if (device_path.length()) {
		std::cout << "Found serial device: " << device_path << std::endl;
	} else {
		std::cerr << "Serial USB device (/dev/ttyUSBn or /dev/ttyACMn) not found. Exiting." << std::endl;
	}
}

FindSerialUsb::~FindSerialUsb() {
}

} /* namespace application */
