/**
 * LED array development sketch. We will evolve this code to flash messages
 * on an 8 x 32 LED matrix. Logically speaking, this is the main program.
 *
 * Note that Sloeber.ino is the actual main program that is generated by the
 * Sloeber development environment. You can download Sloeber from
 *
 *   https://eclipse.baeyens.it/
 *
 */
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "CanonicalColors.h"
#include "CommandIndicator.h"
#include "CommandPublisher.h"
#include "ColorTable.h"
#include "DisplayDrivers.h"
#include "DisplayManager.h"
#include "DisplayTask.h"
#include "FastLED.h"
#include "I2CCommandHandler.h"
#include "Marquee.h"
#include "Panels.h"
#include "SerialReadTask.h"
#include "SPIReceiveTask.h"

#include "PwmPinsAndChannels.h"

#define BUILTIN_LED 2
#define HONORING_CONTRACTS_NOT 4

#define NUM_LEDS (8 * 32)

#define MAX_SPI_MESSAGE 128

WORD_ALIGNED_ATTR DMA_ATTR uint8_t spi_receive_buffer[MAX_SPI_MESSAGE];
WORD_ALIGNED_ATTR DMA_ATTR uint8_t spi_send_buffer[MAX_SPI_MESSAGE];

static CRGB leds[NUM_LEDS];
static 	const Panel* panel = Panels::alternatingEightByThirtyTwo();

static Marquee marquee(leds, NUM_LEDS, *panel);
static CommandIndicator command_indicator;
static DisplayDrivers drivers;
static DisplayManager display_manager(&drivers, &marquee);

static CommandPublisher command_publisher;
static I2CCommandHandler i2c_command_handler(&command_publisher);
// static I2CReceiveTask i2c_receive_task(&command_publisher);
static SPIReceiveTask spi_receive_task(
    &command_publisher,
    MAX_SPI_MESSAGE,
    spi_receive_buffer,
    spi_send_buffer);
static SerialReadTask read_task(&command_publisher);


static QueueHandle_t h_command_queue;
static QueueHandle_t h_indicator_queue;
static TaskHandle_t h_command_indicator;
static TaskHandle_t h_display_manager;
static TaskHandle_t h_serial_communication_manager;

static void start_display_manager(void *pv_params) {
	display_manager.begin(h_command_queue, h_indicator_queue);
}

static void start_serial_task(void *pv_params) {
	read_task.run();
}

//The setup function is called once at startup of the sketch
void setup() {
  Serial.begin(115200);
  Serial.setRxBufferSize(256);
  pinMode(BUILTIN_LED, OUTPUT);

  // The sketch cannot honor its contracts until it is initialized
  pinMode(HONORING_CONTRACTS_NOT, OUTPUT);
  digitalWrite(HONORING_CONTRACTS_NOT, HIGH);

  Serial.print("Team 7587, Metuchen Momentum marquee compiled on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);
  Serial.println("LED array test: serial I/O initialized.");
  FastLED.addLeds<WS2812, LED_ARRAY_PIN, GRB>(leds, NUM_LEDS);
  Serial.println("LED array initialized.");

  h_command_queue = xQueueCreate(4, sizeof(DisplayMessage));
  h_indicator_queue =
      xQueueCreate(4, sizeof(CommandIndicator::CommandIndicatorMessage));

  h_command_indicator = command_indicator.start(h_indicator_queue);
  command_publisher.begin(h_command_queue);

  xTaskCreate(
    start_display_manager,
    "Display manager",
    1024,
    NULL,
    2,
    &h_display_manager);

  xTaskCreate(
    start_serial_task,
    "Serial communication manager",
    1024,
    NULL,
    3,
    &h_serial_communication_manager);

  spi_receive_task.start();

  i2c_command_handler.begin();

  DisplayMessage left_to_right_message;
  left_to_right_message.p_text = NULL;
  left_to_right_message.text_length = 0;
  left_to_right_message.command = DisplayCommand::SINGLE_PIXEL_LEFT_TO_RIGHT;
  left_to_right_message.delay1 = 0;
  left_to_right_message.delay2 = 0;
  left_to_right_message.foreground = CRGB::Red;
  left_to_right_message.background = CRGB::Black;
  xQueueSendToBack(
    h_command_queue, &left_to_right_message, pdMS_TO_TICKS(1));

  digitalWrite(HONORING_CONTRACTS_NOT, LOW);
  Serial.println("Initialization complete, honoring all contracts.");
}

// The loop function is called in an endless loop
void loop() {
	//Unused by FreeRTOS
  vTaskDelete(NULL);
}
