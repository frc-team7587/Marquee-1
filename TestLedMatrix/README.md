# Metuchen Momentum Marquee Display

## Overview

The Metuchen Momentum Marquee Display is a program that manages a
display marquee consisting of an array of individually addressable
LED pixels. It has the following characteristics:

 - **Programming language**: C++
 - **Target hardware**: [ESP32](https://www.espressif.com/en/products/socs/esp32)
 - **Runtime environment**: [ESP FreeRTOS Port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html),
 - **Development environment**: [Sloeber](https://eclipse.baeyens.it/)
 - **Runtime library**: [Arduino](https://www.arduino.cc/reference/en/libraries/)
 - **Display libaray**: [FastLED](https://fastled.io/docs/3.1/)
 
Developers need to be familiar with the foregoing.

## Development Environment

The code base was written with the
[Sloeber](https://eclipse.baeyens.it/) IDE, though any
ESP32 IDE can be used. The development team selected Sloeber
because:

1. The initial developer uses Eclipse, so was comfortable with
   its key bindings and UI.
2. Sloeber manages multiple files well.
3. Sloeber is a (relatively) mature IDE.

While the Arduino IDE would work in principle, it does not
handle multiple source files well, so might not be the most
convenient environment. That said, use whatever suits you.

## Architecture

The system contains the following components

1. Generated files, especially Sloeber.ino. The Sloeber IDE generates this file.
   Do not alter it.
2. System assembly and startup, TestLedMatrix.ino. Code in this module
   configures I/O, starts all required tasks, and sets the initial panel
   display.
3. A command parser, which decodes and validates received commands,
   transforms the command into a semaitcally equivalent internal message,
   and sends the message to the Display task (see below). 
4. Serial communication task, which reads display commands from the main
   serial port (the ESP32 has multiple serial port. The main port is the
   one on the USB connection.) and forwards the received command to the
   command decoder. The Serial Communications Task runs at high relative priority.
5. SPI communications task, which reads commands over SPI. Like the serial
   communications task, it runs at high priority, and forwards its input
   to the command decoder.
6. Display task, which receives and responds to internal display messages and
   adjusts the display as directed. This task runs at relatively low priority.
7. [FreeRTOS Queue](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#queue-api)
   used for inter-task communications, to send display directives from the
   Serial communications task to the display task.

The serial and SPI communication tasks runs at relatively high priority so the
system can respond to commands as quickly as possible. Even though the
communication tasks run at high priority, they spends most of their
time waiting for input, so they consume little CPI time. The display task,
which runs at lower priority, runs far more frequently and so consumes
more CPU and other resources.

## Adding a Display Driver

Each display type is implemented in a display driver. For example, the DisplayFillDriver,
which is implemented in `DisplayFillDriver.h` and `DisplayFillDriver.cpp` sets all
pixels to a specified color.

All display drivers:

* Publicly inherit the `DisplayDriver` class that is defined in `DisplayDriver.h` and
  `DisplayDriver.cpp`
* Have a corresponding enumeration in the `DisplayCommand` enumeration. For example,
  the `DisplayFillDriver`'s enumeration is `FILL_WITH_COLOR`.
* Implement the following two methods, which are pure virtual methods in the
  `DisplayDriver` class:
  1. `begin()`, which performs all required setup and creates the initial display.
  2. `refresh()`, which refreshes the display. This method optionally updates the
      matrix to display animation effects. See, e.g. `SinglePixelLeftToRight` for
      usage.
* Have a static instance defined in `DisplayDrivers.cpp`.
* Are returned by a case in `DisplayDrivers::of_type()`.
* Have a DisplayCommand in the command table in `CommandPublisher.cpp`.

C++ enumerations have integer values which, unless otherwise specified,
start from 0. This is the display command value that specifies the triver.

:**NOTE** When you add a value to the `DisplayCommand` enumeration,
       make sure that `NUMBER_OF_COMMANDS` remains in **LAST** place.
       Failure to to so will cause undefined behavior.

`DisplayDriver`, the class that manages the display, passes a queue handle to
the  `DisplayDriver`'s `begin()` and `refresh()` methods. Both methods must
query the queue for an unspecified but non-zero period and return the query's
result. The delay is implementation-dependent, and can be a constant, as in
`DisplayFillDriver` or taken from the incoming `DisplayMessage`. The latter
is recommended for all new anamated displays.

Note that `DisplayDriver` invokes `Display::refresh()` over and over until
the method indicates that an incoming command has arrived by returning `pdPASS`.
`xQueuePeek()` takes care of this automatically. Please see its
[Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html?highlight=xqueuepeek#_CPPv410xQueuePeek13QueueHandle_tPCv10TickType_t)
for details.


## Hardware

The breadboard system uses the following hardware:

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
My configuration is running without them, but that might be blind dumb luck.

We must ruggedize the hardware before deployment.

## Wiring the marquee

ESP32 GPIO pins are assigned in `PwmPinsAndChannel.h`, which specifies that
GPIO Pin 15 is connected to the LED array signal line. At the time of writing,
this is the only pin that the software uses. 

There are three wiring configurations:

1. Power provided over USB
2. 5 Volt power provided directly to the board.
3. 3.3 Volt power provided directly to the board.


:warning: the Robot's 5V power supply does not provide usable power, and the
RoboRio's USB port also cannot power the ESP32. The power wiring must conform
to FIRST requirements, which might require us to install a separate 12 Volt to
5 Volt converter to power the microcontroller.

:warning: **WARNING** Refer to the [pinout diagram](ESP32ModulePinout.jpg) for pin assignments.
Check every connection twice before applying power. The pin numbers apply
ONLY to the to the following 
[ESP32 Development Board](https://www.amazon.com/gp/product/B09DPGMZR9/ref=ppx_yo_dt_b_asin_title_o06_s00).

:warning: **WARNING** The ESP32 chip must be powered at 3.3 Volts only, and will be destroyed
if 5 Volts is applied to ANY pin **EXCEPT** 5V. The 5V pin connects to a voltage regulator
that reduces 5 Voltes to 3.5, which allows the board (**NOT** the chip)
to run on 5 Volt power. All 5 Volt power, including power provided via
USB, MUST be routed to the regulator. Be **EXTREMELY CAREFUL** when supplying
power to the board.

Note that we must connect all grounds together. This includes

- ESP32 Groound
- Power supply ground
- LED array ground

### Wiring the Marquee

The Marquee has three sets of wires:

* Red, green, and white signal output wires atached to a 3 pin female connector. These are used
  for daisy chaining panels. We don't use them.
* Unterminated (no connector attached) red and black wires for power. We apply power here.
* Red, green, and white signal input wires attached to a 3 pin male connecter for signal and
  optional power input. We only use the signal wires.
  
Wires are labeled where they attach to the panel. The power wire labels are

| Color | Label | Description   |
| ----- | ----- | ------------- |
| Red   | 5V    | +5 Volt power |
| Black | GND   | Ground        |

Connect the power wires directly to a 5 Volt 75 Watt power source. Do not connect
them to the ESP32. 

Signal input wires are labeled and connected to the ESP32 as follows:

| Color | Label | ESP32 Pin | Description                       |
| ----- | ----- | --------- | --------------------------------- |
| Green | DIN   | GPIO 15   | LED configuration command stream. |
| White | GND   | GND       | Signal ground                     |
| Red   | 5V    | N/C       | Not connected.

:note: **NOTE** The LED matrix control signals run at 800 KHz, a typical frequency for an AM radio station.
(WNYC AM broadcasts at 820 KHz.) Control signals must be hard wired to the three pin female LED matrix
control socket. Be sure to solder all joints. Lever style connectors render the connection useless.

As stated above, the signal output wires are unused. Let them dangle.

### Power over USB

As stated above, the RoboRio cannot power the ESP32. We provide power by another means
and use the USB only for communication.

### Direct Connect 5 Volt Power

Wire the chip as follows to power the ESP32 from the 5 Volt power supply. We will do
this only during development, and only if the USB cable cannot provide enough power.
To comply with FIRST specification, we **WILL NOT** use this on the robot.

| Pin Number | Pin Name | Connect To |
| ---------- | -------- | ---------- |
|         19 | Vin 5V   | 5 Volt Power |
|         38 | GND      | Plugboard ground (blue row on the solderless breadboard power strip) |

### Direct Connect 3.3 Volt Power

When we install the Marquee on the robot, we will power the ESP32 from the PDP's 3.3
power supply.

| Pin Number | Pin Name | Connect To |
| ---------- | -------- | ---------- |
|          1 | V 3.3    | 3.3 Volt Power (from the PDP when installed) |
|         38 | GND      | Plugboard ground (blue row on the plugboard power strip) |

### Communications With the RoboRio

We have elected to communicate via USB. The ESP32 emulates an [RS232](https://en.wikipedia.org/wiki/RS-232)
connect that is also called a `tty`. The ESP32 emulates a [serial port](https://en.wikipedia.org/wiki/Serial_port) 
that is configured as follows

| Parameter | Value  | Description |
| --------- | ------ | ----------- |
| Baud      | 115200 | Speed in bits/second, incorrectly called the [baud rate](https://en.wikipedia.org/wiki/Baud) for historic reasons. |
| Parity    | None   | The parity bit, the meaning of the character code's most significant bit. |
| Stop Bits | 1      | The number of stop bits, the bit(s) that signal the end of a character. |
| Data Bits | 8      | The number of bits in a character. |
 
