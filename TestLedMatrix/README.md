# Metuchen Momentum Marquee Display

## Overview

The Metuchen Momentum Marquee Display is a program that manages a
display marquee consisting of an array of individually addressable
LED pixels. It has the following characteristics:

 - **Programming language**: C++
 - **Target hardware**: [ESP32](https://www.espressif.com/en/products/socs/esp32)
 - **Runtime environment**: [ESP32 FreeRTOS Port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html),
 - **Development environment**: [Sloeber](https://eclipse.baeyens.it/)
 - **Runtime library**: [Arduino](https://www.arduino.cc/reference/en/libraries/)
 - **Display libaray**: [FastLED](https://fastled.io/docs/3.1/)
 
Developers need to be familiar with the foregoing.

## Installation


To build and install the Marquee application:

1. Ensure that you have an Arduino IDE available.
2. Wire the ESP32 to the Display Panel as described
   [below](https://github.com/frc-team7587/Marquee-1/blob/main/TestLedMatrix/README.md#wiring-the-display-panel).
3. Wire the RoboRio to the ESP32, also described 
   [below](https://github.com/frc-team7587/Marquee-1/blob/main/TestLedMatrix/README.md#wiring-the-display-panel).
   If you are using both the 5 Volt and 3.3 Volt buck converters (not recommended),
   **BE SURE TO WIRE THEM CORRECTLY** as any mistake could destroy the ESP32. The same caveat
   applies when daisy chaining ESP32 power as recommended.

4. Incorporate the Marquee Client library into the robot control software.
   Invoke the API as desired.
5. Enjoy the fruits of your labor.

## Development Evironment

An Arduino development IDE is needed to build the Marquee software and upload it
to the ESP32. We have used
[Sloeber](https://eclipse.baeyens.it/),
an
[Eclipse](https://www.eclipse.org/)-based
IDE. The standard
[Arduino](https://www.arduino.cc/)
[IDE](https://www.arduino.cc/en/software)
will compile and install the software, but the project is too complex to
develop on the Arduino IDE. There are other widely used developmment
environments like
[VSCode](https://code.visualstudio.com/download) with
[PlatformIO](https://platformio.org/install/ide?install=vscode).
Whichever you choose, be sure to install the ESP32
Arduino libraries and to configure it to target the ESP32 S2.

### IDE

The code base was written with
[Sloeber](https://eclipse.baeyens.it/) version 4.4.3 .

As stated above, any ESP32-compatible IDE can be used. The development team
selected Sloeber because:

1. The initial developer uses Eclipse, so was comfortable with
   its key bindings and UI.
2. Sloeber manages multiple files well.
3. Sloeber is a (relatively) mature IDE.

Sloeber needs a simple tweak to be usable. Please reach out for
details.

<!--- TODO: provide link! -->

### Libraries

The project requires the following libraries

* ESP32 version 3.1.1. Note that this library includes `FreeRTOS`.
* [FastLED version 3.9.13](https://github.com/FastLED/FastLED/releases).
* [SPI version 3.1.1](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/spi.html)
* [Wire (i.e. I2C) version 3.3.1](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/i2c.html)

All were installed via Sloeber's 
[Library Manager](https://duckduckgo.com/?q=sloeber+library+manager&t=newext&atb=v410-1&ia=web).

## Architecture

The system contains the following components

1. Generated files, especially `Sloeber.ino`. The Sloeber IDE generates this file.
   Do not alter it.
2. System assembly and startup, `TestLedMatrix.ino`. Code in this module
   configures I/O, starts all required tasks, and sets the initial panel
   display.
3. A command parser, which decodes and validates received commands,
   transforms the command into a semaitcally equivalent internal message,
   and sends the message to the Display task (see below). 
4. Serial communication task, which reads display commands from the
   serial USB connection (the ESP32 has multiple serial ports, one of which
   is connected to a UART to USB bridge) and forwards them to the
   command decoder. The Serial Communications Task runs at high relative priority
   so it can respond quickly. Since it spends most of its time waiting for a
   command, it doesn't interfere with display management.
5. SPI communications task, which reads commands over SPI. Like the serial
   communications task, it runs at high priority, and forwards its input
   to the command decoder.
6. I2C communication task which forwards commands received via I2C.
7. Display task, which receives and responds to internal display messages and
   adjusts the display as directed. This task runs at relatively low priority.
8. [FreeRTOS Queue](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html#queue-api)
   used for inter-task communications, to send display directives from the
   Serial communications task to the display task.

Note that the the serial and SPI communication tasks are disabled in the
current version. To enable them:

1. Include their headers, `SPIReceiveTask.h` or `I2CCommandHandlere.h` into
   `TestLedMatrix.ino`.
2. Create static instances of the desired class(es).
3. Start the task.

See below for details.

### I2C

arrow_right:**NOTE**: I2C is disabled in the current version.
Add the following to enable it.

To create the task:
```1
i2c_receive_task(&command_publisher);
```

To start the task:

```
  i2c_command_handler.begin();
```

### SPI

arrow_right:**NOTE**: SPI is disabled in the current version.
Add the following to enable it.

To create the task:

```
static SPIReceiveTask spi_receive_task(
    &command_publisher,
    MAX_SPI_MESSAGE,
    spi_receive_buffer,
    spi_send_buffer);
```

Initialize the SPI task pins before starting the task.

```
SPIReceiveTask::configure_gpio();
```

Note that `SPIReceiveTask::configure_gpio` is a static method.

To start the task:

```
spi_receive_task.start();
```

### Receive Task Priorities

Receive tasks run at relatively high priority so the
system can respond to commands as quickly as possible. Even though the
communication tasks run at high priority, they spends most of their
time waiting for input, so they consume little CPU time. The display task,
which runs at lower priority, runs far more frequently and so consumes
more CPU cycles.

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

:arrow_right:**NOTE**: When you add a value to the `DisplayCommand` enumeration,
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

- The [AITRIP ESP32 Development Board](https://www.amazon.com/gp/product/B09DPGMZR9/ref=ppx_yo_dt_b_asin_title_o06_s00)
  These were selected on price and recommendation. Other ESP32-based development
  boards could work equally well.
- A [WW2812B](https://www.mouser.com/pdfDocs/WS2812B-2020_V10_EN_181106150240761.pdf)-based
  8 x 32 LED panel similar to [this](https://www.amazon.com/gp/product/B088BTXHRG/ref=ask_ql_qh_dp_hza).
- A 5 volt power supply. Mine was salvalged from a server. 
  [This](https://www.amazon.com/dp/B01LXN7MN3?ref_=ppx_hzsearch_conn_dt_b_fed_asin_title_5) works well. If
  you boy one, be sure to buy compatible sockets (see below)
  as well.
- [5.5mm x 2.1mm DC Power Jack](https://www.amazon.com/Chanzon-Female-Connector-Security-Adapter/dp/B079RCNNCK/ref=sr_1_45),
  the mate for the power supply's jack. These connectors are ubiquitous. In addition to powering the marquee,
  we power RoboRios when they are on the bench.
- An [830 Point solderless breadboard](https://www.amazon.com/EL-CP-003-Breadboard-Solderless-Distribution-Connecting/dp/B01EV6LJ7G/ref=sr_1_27)
  for prototyping
- A [12 to 5 VDC Buck Converter](https://www.amazon.com/Regulator-Converter-12V-Waterproof-Transformer/dp/B08CHMJM9J/ref=sr_1_13)
  to power the array and ESP32 on the robot

arrow_right:**NOTE**: The ESP32 runs at 3.3 volts and the display runs at 5 volts.
We've found that the ESP32 controls the panel withoyut issue, but that might be blind dumb luck.
If voltage compatiblity causes problems, logic 
[level shifters](https://www.amazon.com/SongHe-Channels-Converter-Bi-Directional-Shifter/dp/B07YZTW2SM/ref=sr_1_4)
might fix it.

We must ruggedize the hardware before deployment.

## Wiring the marquee

ESP32 GPIO pins are assigned in `PwmPinsAndChannel.h`, which specifies that
GPIO Pin 15 is connected to the LED array signal line. At the time of writing,
this is the only pin that the software uses.

There are three wiring configurations:

1. Power provided over USB
2. 5 Volt power provided directly to the board.
3. 3.3 Volt power provided directly to the board.


:warning: **WARNING** the VRM's 5V power supply cannot power the ESP32. If
you value your VRM, don't even think about connecting it to the microcontroller.

:warning: **WARNING** Refer to the [pinout diagram](ESP32ModulePinout.jpg) for pin assignments.
Check every connection twice before applying power. The pin numbers apply
ONLY to the to the following 
[ESP32 Development Board](https://www.amazon.com/gp/product/B09DPGMZR9/ref=ppx_yo_dt_b_asin_title_o06_s00).

:warning: **WARNING** The ESP32 chip runs only on 3.3 Volts, and will be destroyed
if 5 Volts is applied to ANY pin **EXCEPT** the one labeled 5V. This pin connects
to a voltage regulator that reduces 5 Volts to 3.5, which allows the board (**NOT** the chip)
to run on 5 Volt power. All 5 Volt power, including power provided via
USB, MUST be routed to the regulator. Be **EXTREMELY CAREFUL** when supplying
power to the board. Reversing polarity will ruin the chip.

Be sure to connect all grounds together, including

- ESP32 Groound
- Power supply ground (i.e. buck converter(s))
- LED array ground

Fortunately, ground wiring is straightforward. The power supply ground connects
directly to the LED array ground, and ground is daisy chained
to the ESP32.

Note that the ESP32 has three ground pins: 14, 32, and 38. They are interchangeable.

### Powering the ESP32

As stated above, the best way to power the ESP32 is to daisy chain it to
LED array power.

:warning: **BE SURE** to daisy change LED array power to
its 5 Volt pin. Connecting 5 Volts to any other pin **will destroy the microcontroller**.

### Wiring the Display Panel

The Marquee has three sets of wires:

* Red, green, and white signal output wires atached to a 3 pin female connector. These are used
  for daisy chaining panels. Since we only have a single panel, we don't need them. Just tuck
  them out of the way.
* Unterminated (no connector attached) red and black wires for power. We apply power here.
* Red, green, and white signal input wires attached to a 3 pin male connecter for signal and
  power. These provide power to the ESP32 and commands to the LED array.

Wires are labeled where they attach to the panel. The power wire labels are

| Color | Label | Description   |
| ----- | ----- | ------------- |
| Red   | 5V    | +5 Volt power |
| Black | GND   | Ground        |

Connect the power wires directly to a 5 Volt 75 Watt power source. Double check polarity
before powering up the panel, as connecting the power backward will wreck the panel.

Connect signal in and power out wires to the ESP32 as follows:

| Color | Label | ESP32 Pin | Description                       |
| ----- | ----- | --------- | --------------------------------- |
| Green | DIN   | GPIO 15   | LED configuration command stream. |
| White | GND   | GND       | Signal ground                     |
| Red   | 5V    | 5V        | ESP32 5V                          |

:arrow_right:**NOTE**: The LED matrix control signals run at 800 KHz, a typical frequency for an AM radio station.
(WNYC AM broadcasts at 820 KHz.) Control signals must be hard wired to the three pin female LED matrix
input socket. Keep the signal wires short. We recommend mounting the ESP32 on the the Marquee's back.

### Power over USB

As stated above, the RoboRio can power the ESP32. It cannot, however, power
the panel. USB power works for benchtop development, but we recommend daisy
chaning power as described above and using the USB purely for signal.

### Direct Connect 3.3 Volt Power

It is possible to power the ESP32 from its own 3.3 Volt buck converter, however
this configuration is not recommended as daisy chaining 5 Volt power is simpler
and works equally well.

| Pin Number | Pin Name | Connect To |
| ---------- | -------- | ---------- |
|          1 | V 3.3    | 3.3 Volt Power (from the PDP when installed) |
|         38 | GND      | Plugboard ground (blue row on the plugboard power strip) |

We include this for the sake of completeness.

## Communications With the RoboRio

The sender provides a newline ('\n')-terminated, pipe ('|') delimited, 8 bit
[US ASCII](https://www.charset.org/charsets/us-ascii)
string whose fields contain the following.

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

arrow_right:**NOTE**: the terminating newline follows **immediately** after
the final field. There must **NOT** be an interviening pipe. A return ('\r') **MAY**
precede the newline.

Please see `SerialReadTask.h` for further details.

The ESP32 supports three input types.

1. Serial [USB](https://en.wikipedia.org/wiki/USB) terminal emulation
2. [SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface) if enabled (see above)
3. [I2C](https://en.wikipedia.org/wiki/I%C2%B2C) if enabled (see above)


Each has unique characteristics, including wiring and limitations.
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

The connection transmits 11520 characters/second, more than adequate for controlling the LED array,
but slow in comparison to SPI or Ethernet.

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

### I2C

The ESP32 accepts input over its built-in 
[Inter-Integrated Circuit](https://en.wikipedia.org/wiki/I%C2%B2C)
interface. Connect the devices as follows:

| Signal Name          | RoboRio Pin | ESP32 GPIO Pin     | 
| -------------------- | ----------- | ------------------ |
| Clock                | SCL         | GPIOP 22 (I2C SCL) |
| Data                 | SDA         | GPIOP 31 (I2C SDA) |
| Ground               | Ground      | Ground             |


Like SPI, the I2C interface accepts fixed length 128 byte messages.
Zero fill the message on the right,

Note that the ESP32 must be fitted with 4.7 KOhm pullup
resistors on I2C SCL and I2C SDA. Since the RoboRio has
internal pullup resistors, it does not require external
resistors.

