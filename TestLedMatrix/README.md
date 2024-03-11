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

**NOTE** When you add a value to the `DisplayCommand` enumeration,
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
power to the board. Reversing polarity will ruin the chip.

Note that we must connect all grounds together. This includes

- ESP32 Groound
- Power supply ground (i.e. buck converter(s))
- LED array ground

The ESP32 has three ground pins: 14, 32, and 38. They are interchangeable.

### Powering the ESP32

As indicated above, the ESP32 can be powered three ways.

1. Via its USB connection. Note that the RoboRio USB ports can
   power the ESP32.
2. Directly from a 3.3 Volt power source (i.e. buck converter)
3. Directly from a 5 Volt power source, which **must** be connected to the
   ESP32's 5 Volt pin.
   
:warning: if the ESP32 is powered by 5 Volts, **BE SURE** to connect power to
its 5 Volt pin. Connecting 5 Volts to any other pin **will destroy the microcontroller**.

The LED panel's red wire can power the ESP32. Just connect it to
**the ESP32's 5 Volt pin**. This is the recommended configuration
because it guarantees that the ESP32 is powered on **before** the
RoboRio searches for it.

### Wiring the Display Panel

The Marquee has three sets of wires:

* Red, green, and white signal output wires atached to a 3 pin female connector. These are used
  for daisy chaining panels. Since we only have a single panel, leave them be and tuck them out
  of the way. 
* Unterminated (no connector attached) red and black wires for power. We apply power here.
* Red, green, and white signal input wires attached to a 3 pin male connecter for signal and
  optional power input. We only use the signal wires.
  
Wires are labeled where they attach to the panel. The power wire labels are

| Color | Label | Description   |
| ----- | ----- | ------------- |
| Red   | 5V    | +5 Volt power |
| Black | GND   | Ground        |

Connect the power wires directly to a 5 Volt 75 Watt power source. Double check polarity
before powering up the panel, as connecting the power backward will wreck the panel. Do
not connect the power wires to the ESP32. 

Signal input wires are labeled and connected to the ESP32 as follows:

| Color | Label | ESP32 Pin | Description                       |
| ----- | ----- | --------- | --------------------------------- |
| Green | DIN   | GPIO 15   | LED configuration command stream. |
| White | GND   | GND       | Signal ground                     |
| Red   | 5V    | N/C       | Not connected.

**NOTE** The LED matrix control signals run at 800 KHz, a typical frequency for an AM radio station.
(WNYC AM broadcasts at 820 KHz.) Control signals must be hard wired to the three pin female LED matrix
control socket. Be sure to solder all joints. Lever style connectors render the connection useless.

As stated above, the signal output wires are unused. Let them dangle.

### Power over USB

As stated above, the RoboRio cannot power the ESP32. We provide power by another means
and use the USB only for communication.

### Direct Connect 5 Volt Power

To power the ESP32 from a 5V supply, Wire the chip as follows

| Pin Number | Pin Name | Connect To |
| ---------- | -------- | ---------- |
|         19 | Vin 5V   | 5 Volt Power |
|         38 | GND      | Plugboard ground (blue row on the solderless breadboard power strip) |

Note that the standard 5V power supply cannot power the ESP32. We must provide
an independent power source.

### Direct Connect 3.3 Volt Power

When we install the Marquee on the robot, we will power the ESP32 from the PDP's 3.3
power supply.

| Pin Number | Pin Name | Connect To |
| ---------- | -------- | ---------- |
|          1 | V 3.3    | 3.3 Volt Power (from the PDP when installed) |
|         38 | GND      | Plugboard ground (blue row on the plugboard power strip) |

## Communications With the RoboRio

The sender provides a newline ('\n')-terminated, pipe ('|) delimited string whose fields
contain the following. Note that the terminating newline f"ollows directly after
the final field. There must **NOT** be an interviening pipe. An optional return ('\r') **MAY**
precede the newline.

The command has the following format:


| Field No. | Contents                                           |
| --------- | -------------------------------------------------- |
|         0 | The text to display, if any                        |
|         1 | The display command                                |
|         3 | Delay 1                                            |
|         4 | Delay 2                                            |
|         5 | Foreground red intensity                           |
|         6 | Foreground green intensity                         |
|         7 | Foreground blue intensity                          |
|         8 | Background red intensity                           |
|         9 | Background green intesity                          |
|        10 | Background blue intensity                          |


Please see `SerialReadTask.h` for further details.

The ESP32 supports three input types.

1. Serial [USB](https://en.wikipedia.org/wiki/USB) terminal emulation
2. [SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
3. [I2C](https://en.wikipedia.org/wiki/I%C2%B2C)


Each has unique characteristics, including wiring, message format, and limitations.
While all three connections are supported, only one must be active at any given time.
Multiple connections are not supported.

The following sections refer to RoboRio interfaces. Please refer to the
[documentation](https://www.ni.com/docs/en-US/bundle/roborio-20-umanual/page/umanual.html).
for details. Note that the RoboRio's ground pins are labeled with the
standard 
[ground symbol](https://www.ni.com/en-us/support/documentation/supplemental/18/chassis--earth-and-signal-grounding--terminology-and-symbols.html#:~:text=Most%20of%20the%20time%20the,chassis%20and%20earth%20ground%2C%20respectively.).

### USB

The ESP32 breakout board has a built-in USB to asychronous converter that emulates an
[RS232](https://en.wikipedia.org/wiki/RS-232) connection, a.k.a. a `tty`. It
expects the messasge described above as a sequence of ASCII characters. Because
the protocol supports variable length I/O, the message must **NOT** be padded.

The ESP32 emulates a [serial port](https://en.wikipedia.org/wiki/Serial_port) 
configured as follows

| Parameter | Value  | Description |
| --------- | ------ | ----------- |
| Baud      | 115200 | Speed in bits/second, incorrectly called the [baud rate](https://en.wikipedia.org/wiki/Baud) for historic reasons. |
| Parity    | None   | The parity bit, the meaning of the character code's most significant bit. |
| Stop Bits | 1      | The number of stop bits, the bit(s) that signal the end of a character. |
| Data Bits | 8      | The number of bits in a character. |
 
Connection is simple: run a high quality USB A to mini cord from the RoboRio to the ESP32. Be
sure to use a high quality cable to ensure a reliable connection. The ESP32 is extremely fussy.

The connection transmits 11520 characters/second, fast enough, but slow by today's standards. 

### SPI

The ESP32 accepts input over its built in Serial Peripheral Interface. SPI
is a high speed protocol that supports 80 MHz, a bit over 8 million
characters/second, far faster than we need. Its high speed imposes
the following requirements:

1. Wires must be kept short and direct. Prefer soldering when joining wires.
   Wagos and their ilk block the signal.
2. Messages must contain 128 bytes containing the command string followed by
   zeros. Limit the command string to 126 characters including the newline.

Please see [Wikipedia](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
for details. Connect the RoboRio to the ESP32 as follows:

| Signal Name          | RoboRio Pin | ESP32 GPIO Pin  | 
| -------------------- | ----------- | --------------- |
| Chip Select          | `CS0`       | GPIOP 5 (SS)    |
| Master Out, Slave In | `COPI`      | GPIOP 23 (MOSI) |
| Master In, Slave Out | `CIPO`      | GPIOP 19 (MISO) |
| Ground               | Ground      | Ground          |

As always, do not connect RoboRio power to the ESP32.

### I2C

The ESP32 accepts input over its built-in 
[Inter-Integrated Circuit](https://en.wikipedia.org/wiki/I%C2%B2C)
interface. Connect the devices as follows:

| Signal Name          | RoboRio Pin | ESP32 GPIO Pin     | 
| -------------------- | ----------- | ------------------ |
| Clock                | SCL         | GPIOP 22 (I2C SCL) |
| Data                 | SDA         | GPIOP 31 (I2C SDA) |
| Ground               | Ground      | Ground             |


As always, do not connect RoboRio power to the ESP32.

Like SPI, the I2C interface accepts fixed length 128 byte messages.
Zero fill the message on the right, then set the last byte to
0xFF, all ones. The input routine uses 0xFF to reestablish
sinc when things go wrong.

Note that the ESP32 must be fitted with 4.7 KOhm pullup
resistors on I2C SCL and I2C SDA. Since the RoboRio has
internal pullup resistors, it does not require external
resistors.

