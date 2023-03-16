#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "ScrollingTextDriver.h"

ScrollingTextDriver::ScrollingTextDriver(const TypeFace& type_face) :
    type_face(type_face),
    column_shift(0) {
}

ScrollingTextDriver::~ScrollingTextDriver() {
}

BaseType_t ScrollingTextDriver::begin(
		const DisplayMessage& displayMessage,
		Marquee *marquee,
		QueueHandle_t h_queue) {
    column_shift = 0;
	return write_text(displayMessage, marquee, h_queue);
}

BaseType_t ScrollingTextDriver::refresh(
        const DisplayMessage& displayMessage,
        Marquee *marquee,
        QueueHandle_t h_queue) {
    return write_text(displayMessage, marquee, h_queue);
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
	int columns_in_string = type_face.char_width() * text_length;
	int columns_in_marquee = marquee->columns();
	int rows = type_face.char_height();

    for (int column = 0; column < columns_in_marquee; column++) {
        // Stops it from displaying columns past the string
        if (column + column_shift => columns_in_string) {
            continue;
        }
        for (int row = 0; row < rows; row++) {
            if (type_face.bit_at(p_text, text_length, row, column + column_shift)) {
                marquee->set_pixel(row, column, &foreground);
            }
        }
    }
    
    // Increment the column shift for next frame
    column_shift++;
	
	marquee->show();
    return xQueuePeek(h_queue, &display_message, pdMS_TO_TICKS(20));
}
