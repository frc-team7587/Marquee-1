#include "Arduino.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>
#include <stdlib.h>

#include "SPI.h"

/**
 * Serial/TTY/RS232 to SPI adapter used to test Metuchen Momentum's Marquee.
 * The sketch reads a line of text from the serial port and forwards the
 * data over SPI.
 *
 * Pins used to drive the the test sender. Note that the SPI pins are the
 * defaults for VSPI, a.k.a. SPI 3.
 *
 * GPIO Pin Assignments
 * ---- --- -----------
 *
 * +-----+--------------------------------------------------------------------+
 * | Pin | Function                                                           |
 * +-----+--------------------------------------------------------------------+
 * |  23 | VSPI Master Out/Slave In (MOSI) (default)                          |
 * +-----+--------------------------------------------------------------------+
 * |  19 | VSPI Master In/Slave Out (MISO) (default                           |
 * +-----+--------------------------------------------------------------------+
 * |  18 | VSPI Clock (SCK) (default)                                         |
 * +-----+--------------------------------------------------------------------+
 * |   5 | VSPI Slave Select (SS) (default)                                   |
 * +-----+--------------------------------------------------------------------+
 * |   4 | /Slave Honoring Contracts                                          |
 * +-----+--------------------------------------------------------------------+
 * |   2 | Liveness Blink                                                     |
 * +-----+--------------------------------------------------------------------+\
 *
 * Note that a the standard SPI full duplex transmission **exchanges** data
 * between the master and slave. The master sends via MOSI while simultaneously
 * receiving via MISO. The slave does the converse. The master sets the
 * bit rate.
 *
 * Documentation:
 * --------------
 *
 * General Arduino API documentation: https://www.arduino.cc/reference/en/
 *
 * The Arduino Serial Peripheral Interface used in this sketch:
 * https://docs.arduino.cc/learn/communication/spi
 *
 * The ESP32 SPI Master Driver (background only):
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html
 */

#define SLAVE_HONORING_CONTRACTS_NOT 4
#define LIVENESS_BLINK 2

#define INPUT_OUTPUT_BUFFER_SIZE 128

unsigned char to_slave_buffer[INPUT_OUTPUT_BUFFER_SIZE];
unsigned char from_slave_buffer[INPUT_OUTPUT_BUFFER_SIZE];

SPIClass vspi(VSPI);

TaskHandle_t h_blink;

/**
 * Halts both CPUs. Called only when a fatal error occurs.
 */
void halt(BaseType_t status, const char * message) {
	Serial.print(message);
	Serial.print(" failed with status: ");
	Serial.println(status);
	Serial.print("Halting ...");
	disableCore0WDT();
	disableCore1WDT();
}

void blink_task(void * params) {
	for (;;) {
		digitalWrite(LIVENESS_BLINK, HIGH);
		vTaskDelay(pdMS_TO_TICKS(250));
		digitalWrite(LIVENESS_BLINK, LOW);
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

/**
 * Creates a FreeRTOS task, prints a success or failure message, and
 * halts on error.
 */
TaskHandle_t create_task(
	TaskFunction_t pxTaskCode,
	const char *const pcName,
	const configSTACK_DEPTH_TYPE usStackDepth,
	void *const pvParameters,
	UBaseType_t uxPriority) {
	TaskHandle_t h_created_task;
	BaseType_t create_status = xTaskCreate(
		pxTaskCode,
		pcName,
		usStackDepth,
		pvParameters,
		uxPriority,
		&h_created_task);
	if (create_status != pdPASS) {
		Serial.print("Create failed for: ");
		Serial.print(pcName);
		Serial.print(" with status: ");
		Serial.println(create_status);
		halt(create_status, pcName);
	} else {
		Serial.print("Task ");
		Serial.print(pcName);
		Serial.println(" created.");
	}
	return h_created_task;
}

/**
 * Zero fills both the send and receive buffers and reads a newline ('\n')-
 * terminated string from the USB serial input. Truncates oversized input
 * and terminates the received input with a newline ('\n') character. Returns
 * the number of bytes read, including the added newline.
 */
size_t read_a_line() {
	size_t number_read = 0;
	memset(to_slave_buffer, 0, sizeof(to_slave_buffer));
	memset(from_slave_buffer, 0, sizeof(from_slave_buffer));
//	number_read = Serial.readBytesUntil(
//			'\n', to_slave_buffer, sizeof(to_slave_buffer) - 1);
//	to_slave_buffer[number_read++] = '\n';
	char input_character = 0;
	while (input_character != '\n') {
		int maybe_input = Serial.read();
		if (maybe_input == -1) {
			vTaskDelay(pdMS_TO_TICKS(1));
		} else {
			input_character = (char) maybe_input;
			if (number_read < sizeof(to_slave_buffer) - 1
					|| input_character == '\n') {
				to_slave_buffer[number_read ++] = input_character;
			}
		}
	}


	return number_read;
}

/**
 * Initializes the microcontroller immediately after power on or reboot.
 */
void setup() {
	// Set the USB serial I/O to 115200 bits/second, 8 bits, no parity,
	// with a 256 byte input buffer and a 10 minute timeout.
	Serial.begin(115200);
	Serial.setRxBufferSize(256);
	Serial.setTimeout(600000);
	Serial.println("TTY to SPI converter booted.");

	// Sets the blink pin to output. The blink pin must be connected to
	// the positive pin of an LED, with the LED negative pin connected to
	// ground with a 510 Ohm resistor.
	pinMode(LIVENESS_BLINK, OUTPUT);
	digitalWrite(LIVENESS_BLINK, LOW);

	// The slave set this voltage low when it is ready to honor its
	// SPI contract.
	pinMode(SLAVE_HONORING_CONTRACTS_NOT, INPUT_PULLUP);

	// Initialize the pins used by the VSPI controller. Note that this
	// might be redundant.
	pinMode(SS, OUTPUT);
	digitalWrite(SS, HIGH);
	pinMode(SCK, OUTPUT);
	pinMode(MOSI, OUTPUT);
	pinMode(MISO, INPUT_PULLUP);

	// Print the default VSPI GPIO pin numbers
	Serial.print("MOSI: ");
	Serial.println(MOSI);
	Serial.print("MISO: ");
	Serial.println(MISO);
	Serial.print("SCK: ");
	Serial.println(SCK);
	Serial.print("SS: ");
	Serial.println(SS);

	// Initialize the VSPI controller
	vspi.begin();
	Serial.println("SPI ready to transmit.");

	h_blink = create_task(
			blink_task,
			"Liveness blink",
			1024,
			NULL,
			1);


	Serial.println("Waiting for receiver to honor contracts.");
	while (digitalRead(SLAVE_HONORING_CONTRACTS_NOT) == HIGH) {
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	Serial.println("Forwarding serial input to SPI.");
}

// The loop function is called in an endless loop. It reads a newline
// terminated string from the USB serial port and forward the line to
// the SPI slave.
void loop() {
	size_t bytes_read = read_a_line();
	if (0 < bytes_read) {
//		Serial.println("Sending ....");
		vspi.beginTransaction(SPISettings());
		digitalWrite(SS, LOW);
		vTaskDelay(pdMS_TO_TICKS(1));
		vspi.transferBytes(to_slave_buffer, from_slave_buffer, bytes_read);
		digitalWrite(SS, HIGH);
		vspi.endTransaction();
//		Serial.println(".... sent!");
	}
}
