/*
 * main.cpp
 *
 *  Created on: Aug 7, 2022
 *      Author: Eric Mintz
 *
 * Main program for the LED Marquee Test Driver. The program
 * sends a sequence of commands that drive the display through
 * a variety of screens.
 */

#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <errno.h> // Error integer and strerror() function
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "FindSerialUsb.h"
#include "UsbStreams.h"

/**
 * Command strings.
 */

#define FILL_WITH_BLACK " |1|0|0|0|0|0|0|0|0\r\n"

// Fill with medium violet
#define FILL_WITH_MEDIUM_VIOLET " |1|0|0|63|0|63|0|0|0\r\n"

// Single pixel natural order
#define SINGLE_PIXEL_NATURAL_ORDER " |3|0|0|0|0|0|0|0|0\r\n"


// Single pixel left to right
#define SINGLE_PIXEL_LEFT_TO_RIGHT "|4|0|0|0|0|0|0|0|0\r\n"

 // Team number in soft blue on a black background
#define TEAM_7587 " 7587|5|0|0|63|0|0|0|0|0\r\n"

// "Team" in medium blue on soft white.
#define TEAM_LITERAL " Team|5|0|0|0|0|128|4|4|4\r\n"

#define GO_MO "GO MO|5|0|0|0|96|96|32|0|0\r\n"

pthread_t h_usb_to_terminal;

static char usb_output_buffer[1024];

typedef struct usb_to_screen_struct {
	int usb_file;
} usb_to_screen_config;

void send_to_usb_and_sleep(int file_handle, const char * command, int sleep_seconds) {
	unsigned long command_length = (unsigned long) strlen(command);
	std::cout << "Sending: >>" << command << "<<" << std::endl << std::flush;
	int number_written = write(file_handle, (const void *)command, command_length);
	fdatasync(file_handle);
	if ((unsigned long) number_written != command_length) {
		std::cerr << "Wrote: " << number_written << ", expected to write: "
				<< command_length
				<< ", error code: " << errno << std::endl << std::flush;
	} else {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	}
	std::this_thread::sleep_for(std::chrono::seconds(sleep_seconds));
}

void *usb_output_to_screen(void *parms) {
	int fd = ((usb_to_screen_config *) parms)->usb_file;
	std::cout << "Output to screen thread running. File descriptor is: "
			<< fd << std::endl << std::flush;
	FILE * usb_file = fdopen(dup(fd), "r");
	memset(usb_output_buffer, '\0', sizeof(usb_output_buffer));
	char *pchar = usb_output_buffer;
	for (;;) {
		int input_byte = getc(usb_file);
		if (input_byte != -1) {
			switch ((char) input_byte) {
				case '\r':
					break;
				case '\n':
					std::cout << usb_output_buffer << std::endl << std::flush;
					memset(usb_output_buffer, '\0', sizeof(usb_output_buffer));
					pchar = usb_output_buffer;
					break;
				default:
					if (pchar < (usb_output_buffer + (sizeof(usb_output_buffer) - 2))) {
						*pchar++ = (char) input_byte;
					}
			}
		}
	}
	std::cout << "USB to Screen Thread exiting." << std::endl << std::flush;
	return NULL;
}

int main(int argc, char **argv) {
	std::cout << "Begin run." << std::endl;
	application::FindSerialUsb usb_finder;
	application::UsbStreams usb_streams(usb_finder.device());
	int output_file_handle = usb_streams.output_file_handle();
	int input_file_handle = usb_streams.input_file_handle();
	std::cout << "USB Output (computer --> ESP32_ File Handle: "
			<< output_file_handle << std::endl << std::flush;
	std::cout << "USB Input (ESP32 --> computer) File Handle: "
			<< input_file_handle << std::endl << std::flush;
	if (0 < output_file_handle && 0 < input_file_handle) {
		usb_to_screen_config screen_config;
		screen_config.usb_file = input_file_handle;
		pthread_create(
				&h_usb_to_terminal,
				NULL,
				usb_output_to_screen,
				(void *)(&screen_config));
		if (0 < output_file_handle) {
			while((!std::cin.rdbuf()->in_avail())) {
				send_to_usb_and_sleep(output_file_handle, FILL_WITH_BLACK, 5);
				send_to_usb_and_sleep(output_file_handle, FILL_WITH_MEDIUM_VIOLET, 10);
				send_to_usb_and_sleep(output_file_handle, SINGLE_PIXEL_NATURAL_ORDER, 60);
				send_to_usb_and_sleep(output_file_handle, SINGLE_PIXEL_LEFT_TO_RIGHT, 60);
				for (int i = 0; i < 10; ++i) {
					send_to_usb_and_sleep(output_file_handle, TEAM_LITERAL, 1);
					send_to_usb_and_sleep(output_file_handle, TEAM_7587, 1);
					for (int j = 0; j < 5; ++j) {
						send_to_usb_and_sleep(output_file_handle, GO_MO, 1);
						send_to_usb_and_sleep(output_file_handle, FILL_WITH_BLACK, 1);
					}
				}
			}
			close(output_file_handle);
		}
	} else {
		std::cout << "USB serial connection could not be opened." << std::endl << std::flush;
	}
	return 0;
}
