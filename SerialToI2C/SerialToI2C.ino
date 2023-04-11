/**
 * Reads a line from the USB serial port and retransmits it over I2C to
 * a fixed slave address. This sketch is used to validate controlling the
 * Marquee over I2C.
 *
 * Note: the slave I2C address must not conflict with the gyro address,
 * which defaults to 0x27, but can be set to 0x28 or 0x40. The team uses
 * the default address. The sketch uses the default I2C pins
 */
#include "Arduino.h"

#include "Wire.h"

#include <stdlib.h>
#include <string.h>

#include "driver/i2c.h"

// Pin assignments

#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_FREQ_HZ 100000

#define I2C_MASTER_PORT I2C_NUM_0

#define I2C_MASTER_TX_BUF_DISABLE 0  /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0  /*!< I2C master doesn't need buffer */
#define SLAVE_ADDRESS 0x0A

#define ACK_CHECK_EN 0x1   /* I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0  /* I2C master will not check ack from slave */
#define ACK_VAL 0x0        /* I2C ack value */
#define NACK_VAL 0x1       /* I2C nack value */

#define FORWARDED_MESSAGE_SIZE 128  // Max message to forward.
#define FORWARDED_MESSAGE_BUFFER_SIZE (FORWARDED_MESSAGE_SIZE + 2)

static unsigned char message_to_forward[FORWARDED_MESSAGE_BUFFER_SIZE];
static uint16_t character_count;
bool overflow = false;

void setup() {
  // Set up serial communications and announce start of run
  // Set the USB serial I/O to 115200 bits/second, 8 bits, no parity,
  // with a 256 byte input buffer and a 10 minute timeout.
  Serial.begin(115200, SERIAL_8N1);
  Serial.setRxBufferSize(256);
  Serial.setTimeout(600000);
  Serial.print("Serial to I2C bridge compiled on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);

  memset(message_to_forward, 0, FORWARDED_MESSAGE_BUFFER_SIZE);
  character_count = 0;

  i2c_config_t i2c_config;
  memset(&i2c_config, 0, sizeof(i2c_config));
  i2c_config.mode = I2C_MODE_MASTER;
  i2c_config.sda_io_num = I2C_MASTER_SDA_IO;
  i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_config.scl_io_num = I2C_MASTER_SCL_IO;
  i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_config.master.clk_speed = I2C_MASTER_FREQ_HZ;
  i2c_config.clk_flags = 0;

  Wire.begin();

  Serial.println("Initialization complete.");
}

// The loop function is called in an endless loop
void loop() {
  if (Serial.available()) {
    uint16_t input_char = Serial.read();
    if (character_count < FORWARDED_MESSAGE_SIZE) {
      message_to_forward[character_count++] = (unsigned char) input_char;
      if ((unsigned char) input_char == '\n') {
        if (!overflow) {
          Serial.print("Forwarding >>>");
          Serial.print((char *) message_to_forward);
          Serial.println("<<<");
          Wire.beginTransmission(SLAVE_ADDRESS);
          Wire.write(message_to_forward, FORWARDED_MESSAGE_SIZE);
          Wire.endTransmission();
        } else {
          Serial.println("Truncated message dropped.");
        }
        memset(message_to_forward, 0, FORWARDED_MESSAGE_BUFFER_SIZE);
        character_count = 0;
        overflow = false;
      }
    } else {
      if (!overflow) {
        Serial.println("Buffer overflow.");
      }
      overflow = true;
    }
  }
}
