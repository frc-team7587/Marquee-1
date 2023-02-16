/*
 * usbstreams.cpp
 *
 *  Created on: Aug 7, 2022
 *      Author: Eric Mintz
 */

#include "UsbStreams.h"

#include <iostream>
#include <errno.h> // Error integer and strerror() function
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

static bool configure_serial_file_descriptor(int fd) {
	bool status = false;
	if (0 < fd) {
		struct termios usb_config;
		// POSIX requires us to retrieve the current USB/Serial configuration
		if ((status = tcgetattr(fd, &usb_config)) == 0) {
			usb_config.c_cflag &= ~PARENB;          // No parity
			usb_config.c_cflag &= ~CSTOPB;          // One stop bit.
			usb_config.c_cflag &= ~CSIZE;           // Clear #bits/char
			usb_config.c_cflag &= CS8;              // 8 bit characters
			usb_config.c_cflag |= ~CRTSCTS;         // Disable RTS/CTS flow control
			usb_config.c_cflag |= (CREAD | CLOCAL); // Turn on READ & ignore ctrl lines (CLOCAL = 1)

			usb_config.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
			usb_config.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

			usb_config.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
			usb_config.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

			usb_config.c_lflag &= ~ECHO;   // Disable echo
			usb_config.c_lflag &= ~ECHOE;  // Disable erasure
			usb_config.c_lflag &= ~ECHONL; // Disable new-line echo

			cfsetispeed(&usb_config, B115200);
			cfsetospeed(&usb_config, B115200);
			status = tcsetattr(fd, TCSANOW, &usb_config) == 0;
		}
	}
	return status;
}

static int open_with_error_report(std::string device_path, int flags) {
	int fd = open(device_path.c_str(), flags);
	if (fd < 0) {
		std::cout << "Error opening " << device_path << " error "
			<< errno << std::endl << std::flush;
	}
}

namespace application {

UsbStreams::UsbStreams(
	std::string device_path) :
	device_path(device_path),
	usb_input_file(open_with_error_report(device_path.c_str(), O_RDONLY)),
	usb_output_file(open_with_error_report(device_path.c_str(), O_WRONLY)) {
	bool status = 0 < usb_input_file && 0 < usb_output_file;

	if (status) {
		status = configure_serial_file_descriptor(usb_output_file);
		if (!status) {
			std::cerr << "Could not configure computer --> ESP32 link, error: "
				<< errno << std::endl << std::flush;
		}
		if (status) {
			status = configure_serial_file_descriptor(usb_input_file);
			if (!status) {
				std::cerr << "Could not configure ESP32 --> computer link, error: "
					<< errno << std::endl << std::flush;
			}
		}
		if (!status) {
			std::cerr << "Closing unconfigurable files." << std::endl << std::flush;
			close(usb_input_file);
			close(usb_output_file);
			usb_input_file = -1;
			usb_output_file = -1;
		} else {
			std::cout << "USB connections successfully configured." << std::endl << std::flush;
			std::cout << "Sending break" << std::endl << std::flush;
			tcsendbreak(usb_output_file, 1000);
		}
	} else {
		std::cerr << "Error opening USB files, computer --> ESP32 handle: "
				<< usb_output_file
				<< ", ESP32 --> computer handle: "
				<< usb_input_file
				<< std::endl << std::flush;
	}
}

UsbStreams::~UsbStreams() {
	if (0 < usb_output_file) {
		close(usb_output_file);
		usb_output_file = -1;
		std::cout << "USB port closed." << std::endl << std::flush;
	}
}

} /* namespace spaminterceptor */
