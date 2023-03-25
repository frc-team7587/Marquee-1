/**
 * Test I2C driver that forwards serial input to I2C using the Wire library.
 */

#include "Arduino.h"

#include "Wire.h"

#define MAX_MESSAGE_LEN 128
#define MESSAGE_BUFFER_LEN (MAX_MESSAGE_LEN + 2)

#define MARQUEE_SLAVE_ADDRESS 0xA

static uint8_t message_buffer[MESSAGE_BUFFER_LEN];
static size_t character_count;
bool overflow;

void setup() {
  Serial.begin(115200);
  Serial.setRxBufferSize(256);
  Serial.print("Serial to Wire forwarder compiled on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);

  memset(message_buffer, 0, MESSAGE_BUFFER_LEN);
  character_count = 0;
  overflow = false;

  if (Wire.begin()) {
    Serial.println("I2C initialized.");
  } else {
    Serial.println("I2C initialization FAILED.");
  }
  Wire.setBufferSize(MESSAGE_BUFFER_LEN);
}

void loop() {
  if (Serial.available()) {
    uint16_t raw_input = Serial.read();
    uint8_t input_byte = (uint8_t) raw_input;
    if (character_count < MAX_MESSAGE_LEN) {
      message_buffer[character_count++] = input_byte;
    } else {
      overflow = true;
    }
    if (input_byte == '\n') {
      Serial.print("Message is >>>");
      Serial.print((char *) message_buffer);
      Serial.println("<<<");
      if (overflow) {
        Serial.println("Message overflowed buffer, discarding ...");
      } else {
        Wire.beginTransmission(MARQUEE_SLAVE_ADDRESS);
        Wire.write(message_buffer, MAX_MESSAGE_LEN);
        Wire.endTransmission();
        Serial.println("Message sent.");
      }

      memset(message_buffer, 0, MESSAGE_BUFFER_LEN);
      character_count = 0;
      overflow = false;
    }
  }
}
