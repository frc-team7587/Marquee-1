#include <Arduino.h>
#include "DisplayDriver.h"
#include "TypeFace.h"
#include "FastLED.h"

class ScrollingTextDriver : public DisplayDriver {
public:
  ScrollingTextDriver(const TypeFace& type_face);
  virtual ~ScrollingTextDriver();

  virtual BaseType_t begin(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

  virtual BaseType_t refresh(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

private:
  BaseType_t write_text(
      const DisplayMessage& displayMessage,
      Marquee *marquee,
      QueueHandle_t h_queue);

  const TypeFace& type_face;
  int16_t column_shift;
};
