# Metuchen Momentum Marquee Display

## Overview

The Metuchen Momentum Marquee Display is a program that manages a
display marquee consisting of an array of individually addressable
LED pixels. It has the following characteristics:

 - **Programming language**: C++
 - **Target hardware**: [ESP32](https://www.espressif.com/en/products/socs/esp32)
 - **Runtime environment**: [ESP FreeRTOS Port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.htm),
 - **Development environment**: [Sloeber](https://eclipse.baeyens.it/)
 - **Runtime library**: [Arduino](https://www.arduino.cc/reference/en/libraries/)
 - **Display libaray**: [FastLED](https://fastled.io/docs/3.1/)
 
Developers need to be familiar with the foregoing.

## Architecture

The system contains the following components

1. Generated files, especially Sloeber.ino. The Sloeber IDE generates this file.
   Do not alter it.
2. System assembly and startup, TestLedMatrix.ino. Code in this module
   configures I/O, starts all required tasks, and sets the initial panel
   display.
3. Serial communication task, which reads display commands from the main
   serial port (the ESP32 has multiple serial port. The main port is the
   one on the USB connection.) The decodes and validates received commands,
   transforms the command into a semaitcally equivalent internal message,
   and sends the message to the Display task (see below). Note that the
   Serial communications task runs at high relative priority.
4. Display task, which receives and responds to internal display messages and
   adjusts the display as directed. This task runs at relatively low priority.
5..[FreeRTOS Queue](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#queue-api)
   used for inter-task communications, to send display directives from the
   Serial communications task to the display task.

The serial communications task runs at relatively high priority so the
system can respond to commands as quickly as possible. Even though the
serial communication task runs at high priority, it spends most of its
time waiting for characters to arrive. The display task, which runs at
lower priority, runs far more frequently.

## Hardware

The system uses the following hardware:

- An [ESP32 Development Board](https://www.amazon.com/gp/product/B09DPGMZR9/ref=ppx_yo_dt_b_asin_title_o06_s00)
  These were selected on price and recommendation. There are other development
  boards that would work equally well.
- A [WW2812B](https://www.mouser.com/pdfDocs/WS2812B-2020_V10_EN_181106150240761.pdf)-based
  8 x 32 LED panel similar to [this](https://www.amazon.com/gp/product/B088BTXHRG/ref=ask_ql_qh_dp_hza).
- A 5 volt power supply. Mine was salvalged from a server/
- An [830 Point solderless breadboard](https://www.amazon.com/EL-CP-003-Breadboard-Solderless-Distribution-Connecting/dp/B01EV6LJ7G/ref=sr_1_27)

We will need a [12 to 5 VDC Buck Converter](https://www.amazon.com/Regulator-Converter-12V-Waterproof-Transformer/dp/B08CHMJM9J/ref=sr_1_13)
to power the array on the robot. Because the ESP32 runs at 3.3 volts and the
display runs at 5, We might also need logic 
[level shifters](https://www.amazon.com/SongHe-Channels-Converter-Bi-Directional-Shifter/dp/B07YZTW2SM/ref=sr_1_4).
My configuration is running without them, but that's just luck.
