/*
 * usbstreams.h
 *
 *  Created on: Aug 7, 2022
 *      Author: Eric Mintz
 *
 * Opens the serial USB connection to the microcontroller
 * on two files, one for input, one for output.
 */

#ifndef USBSTREAMS_H_
#define USBSTREAMS_H_
#include <string>

namespace application {

class UsbStreams {
	std::string device_path;
	int usb_input_file;   // USB input file handle
	int usb_output_file;  // USB output file_handle.
public:
	UsbStreams(std::string device_path);
	~UsbStreams();
	int output_file_handle() {
		return usb_output_file;
	}
	int input_file_handle() {
		return usb_input_file;
	}
};

} /* namespace application */

#endif /* USBSTREAMS_H_ */
