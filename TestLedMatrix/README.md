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
   Serial communications task runs at high relative priority. Note
   that serial communication is used during development. The RoboRio
   will communicate via the SPI bus.
4. SPI communications task, which communicates with the RoboRio via the classic,
   full duplex SPI protocol, which means that the Marquee and RoboRio exchange
   data, byte for byte. The Marquee sends text formatted commands to the
   Marquee, while the Marquee sends zeros to the RoboRio. 
5. Display task, which receives and responds to internal display messages and
   adjusts the display as directed. This task runs at relatively low priority.
6. [FreeRTOS Queue](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#queue-api)
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

## Connecting the LED Panel

The [LED Panel}(https://www.amazon.com/BTF-LIGHTING-Individual-Addressable-Flexible-Controllers/dp/B088BTXHRG?th=1)
needs +5 Volts VCC, ground, and a control signal. It has two input and one output connections. Note that the
red wire always carries VCC and the black whire always connects to ground. When present, the green wire carries
the control signal.

* A pair of unterminated black and red wires in the middle of the panel. Connect the red wire to +5 volts
and the black wire to ground.

* Three wires: red, white, and green connected to a female connector

* Three wires: red, white, and green connected to a male connector

Connect the green wire to GPIO pin 15 and the white wire to ground. Note that GPIO pin number differs from the pin's position
on the board. In our perferred breakout board (see link above), GPIO Pin 15 is connected to pin position 23. Please consult
the board's pinut diagram for details.

The Marquee uses the following signals. We have assigned a wire color to each signal.
For the pit crew's sake, please adhere to the resulting conventions.

| GPIO Pin | Color | I/O | Description |
| -------- | ----- | --- | ----------- |
| GND      | Green | N/A | Ground. The development board has three ground pins. They are equivalent. Connect to the RoboRio's GPIO ground and the panel's signal ground. |
| MOSI     | Blue  | Input | Master Out, Slave In: carries data from the RoboRio to the Marquee |
| MISO     | Yellow | Output | Master In, Slave Out: carries data from the Marquee to the RoboRio. |
| CS       | White | Input | Chip Select: normally set to +3.3 Volts. The RoboRio lowers this pin to start a data exchange. |
| SCK      | Black | Input | Clock: controls the data exchange. Each device sends one bit/clock tick. |
| GPIO15   | Brown | Output | Marquee Signal: sets the pixel values in the display panel. |

## Use

The Marquee takes commands in pipe ('|')-delimited format, and must end with a new-line ('\n'). Commands 
are limited to 124 characters including the terminating new line, and must be
sent in a zero-filled, 128 byte buffer. The ESP32's SPI has a long-standing problem, it sometimes drops
the last four characters in an SPI transmission, so we simply don't use them.

The message contains the following fields:

1. **Text**: the text to display on the marquee
2. **Command**: what to do. See `DisplayCommand.h` for declarations.
   Note that the enumerations have integer values starting from 0.
3. **Delay-1**: the first delay value in milliseconds.
4. **Delay-2**: The second delay in milliseconds.
5. **Foreground Red**: the red intensity for the forground color
6. **Foreground Green**: the green intensity for the forground color
7. **Foreground Blue**: the blue intensity for the foreground color
8. **Background Red**: the red intensity for the background color
9. **Background Green**: the green intensity for the background color
10. **Background Blue**: the blue intensity for the background color
11. **Carriage Return**: an optional terminating carriage return ('\r'), ignored if provided
12. **New Line**: the terminating new line ('\n') character, required

Note that:

*   The message is limited to 124 characters, all in.
*   All numbers are decimal
*   Delays are in milliseconds between 0 and 32767 inclusive
*   The display text can be empty and must not contain '|'.  Only characters
*   between ' ' and '~' (inclusive) EXCEPT '|' are supported.
*   The command number is the index of the corresponding command enumeration *
*   Colors are between 0 and 255 inclusive.
*   The terminating carriage return ('\r') optional and ignored if provided
*   The design is optimized for 115200 baud or 11520 characters per second or
    11.52 characters per millisecond. It is recommended that the serial buffer
    be set to 128 bytes or more to guarantee successful operation.




