# The Marquee Control Library

Welcome to the Marquee Control Library, which is used to send commands
from the RoboRio to the ESP32 that controls an 8 x 32
[WS2812B](https://www.mouser.com/pdfDocs/WS2812B-2020_V10_EN_181106150240761.pdf?srsltid=AfmBOopuEYYediMk5JxBohtVJa-GD2ECh8J74tbpOZGHdz4FKr5r_HJC)
individually addressable LED array.

## Overview

The Marquee Control Library is the client side component of
the Metuchen Momentum Marquee software suite. It runs on
a RoboRio that is connected to the Marquee's ESP32. The
current ESP32 software version only supports USB connectivity.
I2C and SPI drivers are also provided, but they
are currently disabled. Please refer to the ESP32-side
documentation if you wish to enable them.

That said, the RoboRio's I2C I/O is so flaky that client-side
I2C support is currently on hold. When National Instruments resolves
its I2C I/O issues, we will resume development.

## Installation

The Marquee Control Library consists of Java classes that invoke standard
Java and WPILib APIs. Copy the package into your robot software source tree,
then import and reference them as you would any other ordinary Java code.

You will need a USB 2 A to micro cable like
[this](https://www.amazon.com/dp/B07GJJXQ22?th=1). Consider
keeping a replacement or two on hand just in case. Don't
bother with USB 3.x; neither the RoboRio nor the ESP32 support
them.

To install the hardware:

1. Install the Marquee as described in its documentation. The Marquee
   should display its left to right pattern as soon as it receives
   power.
2. Connect the USB cable from the RoboRio and the ESP32.
3. Power cycle the robot to establish a connection between the
   RoboRio and the Marquee.
4. If messages don't appear on the Marquee, reboot the ESP32.

## Classes

The library provides the following classes that send commands
to the ESP32:

* `DisplayCommand`, an `enum` of supported display functions, ranging
  from single pixel display to a full text crawl. Note that its enumeration
  must contain the exactly the same entries defined in the ESP32 control
  software's `DisplayCommand.h` file, and vice versa.
* `DisplayConnection`, which specifies the message sending API.
* `DisplayMessage`, the messsage that the RoboRio sends to the ESP32.
  The message includes foreground and background colors, text to display,
  and other information. Please see the class documentation for details.
* `SpiDisplayConnection`, a `DisplayConnection` implementation that
  sends messages via a
  [Serial Peripheral Interface](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
  (SPI) connection.
* `UsbDisplayConnection`, a `DisplayConnection` that sends commands via a
  USB serial link.
* `UsbSerialDeviceList`, which scans `/dev` for ESP32 serial connections
* `UsbSerialPortBuilder`, which configtures ESP32 serial connections and
  opens them on WPILib `SerialPort` instances.
* `VacuousDisplayConnection`, a do-nothing `DisplayConnection` used when no
  marquee is connected.

### `DisplayCommand`

The `DisplayCommand` enumeration specifies the Marquee's supported
actions. Actions range from moving a single pixel around the array,
features that mainly support hardware level debugging, to a full fledged
text crawl.

:warning: keep the `DisplayCommand` enumeration in sync with the
C++ enum specified in Marquee library's
[`DisplayCommand.h`](https://github.com/frc-team7587/Marquee-1/blob/main/TestLedMatrix/DisplayCommand.h)
file. If you change one, you **must** make the corresponding change in the other.

### `DisplayConnection`

The `DisplayConnection` interface specifies the API that sends messages
to the ESP32. It provides a single method that sends a command.

### `DisplayMessage`

The `DisplayMessage` class simplifies command generation. It provides
convenient defaults for all fields, zero for numeric values and the
empty string for text values, and allows the users to set:

* Text to display, if any
* Display command, what the marquee should do with the provided
  values
* Foreground Red, Green, and Blue, the components of the foreground
  color. Note that text is displayed in the foreground color.
* Background Red, Green, and Blue, the components of the background
  color.
* Delay 1 and Delay 2, delays to apply to the display. Delay meanings
  vary by command. For scrolling text, delay 1 specifies the dwell time
  for the initial display, and delay 2 specifies the interval between
  crawl shifting.

:arrow_right: **NOTE**:  It is strongly recommended not to set any unsused field values
so as to maximize backward compatility if future releases put them
into service.

:arrow_right: **NOTE**: Commands are sequences of printable
[US ASCII](https://www.ascii-code.com/) characters. Each
character occupies one
[`byte`](https://docs.oracle.com/javase/tutorial/java/nutsandbolts/datatypes.html)
containing a value between 32 and 127
inclusive. By contrast, Java characters have
[UTF-8](https://en.wikipedia.org/wiki/UTF-8)
values, some of which (such as the emojis we all know and love)
occupy multiple bytes.

Commands are documented below.

Display commands are sent as pipe delimited sequences of
128 or fewer bytes. The sequence includes the command,
display text (if any), foreground and background colors,
and delays.

Color intensities vary from 0 (completely off) to 255 (full power)
and default to 0. Delays are in ESP32 ticks, where one tick is one
millisecond, and default to 0.

:warning: the LED matrix consumes 75 watts when run at full power,equivalent to 750 watts incandescent. Running at full power
significantly shortens LED lifetime. It is strongly recommended that the sum of red, green, and
blue intensities not exceed 128.

### `SpiDisplayConnection`

The `SpiDisplayConnection` class is a
[`DisplayConnection`](#displayconnection)
implementation that sends messages over
[SPI](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface).
The channel is (i.e. slave select pin) is set at construction.

:arrow_right: **NOTE**: Consider using a USB connection if a gyroscope
is connected to the RoboRio via SPI. Gyroscopes generate heavy traffic.

### `UsbDisplayConnection`

The `UsbDisplayConnection` class is a
[`DisplayConnection`](#displayconnection)
implementation that sends commands over a USB serial connection,
a USB connection that emulates a asychronous serial terminal,
a.k.a. a serial port. Prefer this if possible.

### `UsbSerialDeviceList`

The `UsbSerialDeviceList` finds all serial USB devices, file
paths of the form `/dev/ttyUSBn`, where `n` is 0, 1, or 2.
If it finds exactly one, it is presumed to be an ESP32
controlling a marquee.

### `UsbSerialPortBuilder`

The `UsbSerialPortBuilder` builds `SerialPort` instances that connect to
USB serial devices, which are file
paths of the form `/dev/ttyUSBn`, where `n` is 0, 1, or 2. Note that
the `SerialPort` class is provided by the WPI library. 

### `VacuousDisplayConnection`

The `VacuousDisplayConnection` is a `DisplayConnection` implementation
that does absolutely nothing, which is used when a connection to a
marquee cannot be found. It allows application code to send messages
to a marquee when no physical connection exists.

## Commands

As described above, the [DisplayMessage](#displaymessage)
class is used to assemble and send a command to the marquee.
The patterns that the Marquee can display are specified in
the [DisplayCommand](#displaycommand) enumeration, and are
described below.

Note that times are in ESP32 Ticks, where a tick equals a millisecond.
Fields marked "N/A" are not used, and should not be set. Characters are
8 pixels high and 6 pixels wide, including white space.

Please see
[DisplayDriver.java](https://github.com/frc-team7587/Marquee-1/blob/main/Client/examples/sample/src/main/java/frc/robot/DisplayDriver.java)
for example commands.

### Error

The Marquee displays an error pattern when it receives an ill-formed
command (the usual case) or receives an explicit request to display
the error pattern. The error display consists of a barred rainbow ripple.
The explicit request is described immediately below.

Parameters:

| Parameter  | Contents               |
|------------|------------------------|
| Text       | N/A                    |
| Command    | `DisplayCommand.ERROR` |
| Foreground | N/A                    |
| Background | N/A                    |
| Delay 1    | N/A                    |
| Delay 2    | N/A                    |

Note that the Marquee also displays this when it receives an invalid
command.

### Single Color Fill

The color fill command sets every pixel on the panel to a
specified color. 

Parameters:

| Parameter  | Contents                         |
|------------|----------------------------------|
| Text       | N/A                              |
| Command    | `DisplayCommand.FILL_WITH_COLOR` |
| Foreground | RGB fill color                   |
| Background | N/A                              |
| Delay 1    | N/A                              |
| Delay 2    | N/A                              |

### Flash Two Colors

This command has not been implemented.

### Rippling Rainbow

Displays a rainbow pattern that ripples from left to right. New
colors enter from the left and disappear from the right. 

Parameters:

| Parameter  | Contents                                  |
|------------|-------------------------------------------|
| Text       | N/A                                       |
| Command    | `DisplayCommand.RIPPLING_RAINBOW`         |
| Foreground | N/A                                       |
| Background | N/A                                       |
| Delay 1    | Time between color shifts in milliseconds |
| Delay 2    | N/A                    |

### Single Pixel in Natural Order

Displays a simple test pattern, a single pixel that zigzags through
the display. It was originally written to determine pixel addressing.
Retaining it was simpler than removing it, so in it stayed.

:arrow_right: **NOTE**:  Contrary to its appearance, the display is not a true array.
Rather, it is a line of LEDs that has been folded zigzag fashion
so it looks like an aray. At the very lowest level, pixels are
addressed linearly, indexed from 0 to 255.

The pattern cycles through a sequence of standard colors, changing
colors between traversals. 

Parameters:

| Parameter  | Contents                                    |
|------------|---------------------------------------------|
| Text       | N/A                                         |
| Command    | `DisplayCommand.SINGLE_PIXEL_NATURAL_ORDER` |
| Foreground | N/A                                         |
| Background | N/A                                         |
| Delay 1    | N/A                                         |
| Delay 2    | N/A                                         |

### Single Pixel in Left to Right, Bottom to Top Order

Displays a single pixel that moves left to right on the screen,
starting at the bottom row and moving upwards with each traversal.
Like its natural order sibling, it, too, was written to support
development, namely to verify transformation from an (x, y)
address to a pixel index, and was retained due to developer
lazyness.
The display cycles through a set of predefined colors, changing
color when the pattern restarts.

Parameters:

| Parameter  | Contents                                    |
|------------|---------------------------------------------|
| Text       | N/A                                         |
| Command    | `DisplayCommand.SINGLE_PIXEL_LEFT_TO_RIGHT` |
| Foreground | N/A                                         |
| Background | N/A                                         |
| Delay 1    | N/A                                         |
| Delay 2    | N/A                                         |

### Static Text

Displays up to five characters centered on the panel.

Parameters:

| Parameter  | Contents                                    |
|------------|---------------------------------------------|
| Text       | Text to display, up to 5 characters         |
| Command    | `DisplayCommand.STATIC_TEXT`                |
| Foreground | Text (foreground) color                     |
| Background | Non-Text (background) color                 |
| Delay 1    | N/A                                         |
| Delay 2    | N/A                                         |

For example, to display "7587" in red on a black background, 
set the display text to `"7587"` and foreground red to 63.

### Text Scroll

The text scroll displays the start of the text, then shifts
the text to the left one column at a time.

Parameters:

| Parameter  | Contents                                    |
|------------|---------------------------------------------|
| Text       | Text to display                             |
| Command    | `DisplayCommand.SCROLLING_TEXT`             |
| Foreground | Text (foreground) color                     |
| Background | Non-Text (background) color                 |
| Delay 1    | Time to display the initial text            |
| Delay 2    | Time between column shifts                  |

### Text Crawl

The text crawl shifts text in from the right one column at a
time. Text crawls smoothly to the left until its last character
vanishes completely, whereupon scroll starts again.


Parameters:

| Parameter  | Contents                                    |
|------------|---------------------------------------------|
| Text       | Text to display                             |
| Command    | `DisplayCommand.TEXT_CRAWL`                 |
| Foreground | Text (foreground) color                     |
| Background | Non-Text (background) color                 |
| Delay 1    | Time between column shifts                  |
| Delay 2    | N/A                                         |


