#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "ScrollingTextDriver.h"


#define DEFAULT_INITIAL_DELAY_TICKS 1000
#define DEFAULT_INTERFRAME_DELAY_TICKS 90

ScrollingTextDriver::ScrollingTextDriver(const TypeFace& type_face) :
    type_face(type_face),
    initial_delay(0),
    interframe_delay(0),
    column_shift(0) {
}

ScrollingTextDriver::~ScrollingTextDriver() {
}

BaseType_t ScrollingTextDriver::begin(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
    column_shift = 0;
    initial_delay = displayMessage.delay1
        ? displayMessage.delay1
        : DEFAULT_INITIAL_DELAY_TICKS;
    interframe_delay = displayMessage.delay2
        ? displayMessage.delay2
        : DEFAULT_INTERFRAME_DELAY_TICKS;
    marquee->flood(&displayMessage.background);
    marquee->show();
    return xQueuePeek(h_queue, &display_message, 0);

}

BaseType_t ScrollingTextDriver::refresh(
    const DisplayMessage& displayMessage,
    Marquee *marquee,
    QueueHandle_t h_queue) {
  return write_text(
      displayMessage,
      marquee,
      h_queue);
}

BaseType_t ScrollingTextDriver::write_text(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue) {
  size_t led_count = marquee->led_count();
  CRGB background = displayMessage.background;
  CRGB foreground = displayMessage.foreground;

  marquee->flood(&background);

  const unsigned char *p_text = displayMessage.p_text;
  size_t text_length = displayMessage.text_length;
  uint16_t columns_in_string = type_face.char_width() * text_length;
  uint16_t columns_in_marquee = marquee->columns();
  uint16_t delay_in_ticks = column_shift
      ? interframe_delay
      : initial_delay;
  uint16_t rows = type_face.char_height();
  uint16_t available_columns = columns_in_string - column_shift;
  uint16_t columns_to_display =
      available_columns < columns_in_marquee
      ? available_columns
      : columns_in_marquee;

  for (uint16_t column = 0; column < columns_to_display; column++) {
    for (uint16_t row = 0; row < rows; row++) {
      if (type_face.bit_at(p_text, text_length, row, column + column_shift)) {
        marquee->set_pixel(row, column, &foreground);
      }
    }
  }

  column_shift++;
  if (columns_in_string <= column_shift) {
    column_shift = 0;
  }

  marquee->show();
  return xQueuePeek(h_queue, &display_message, delay_in_ticks);
}
