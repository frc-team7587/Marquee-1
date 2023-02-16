# The C++ test driver for the Metuchen Momentum LED Marquee

## Overview

The C++ test marquee driver that cycles the FIRST Team 7587,
Metuchen Momentum LED Marquee through a sequence of images.
Because it's written in C++, it can run from computers that
cannot support Java. We need to write an analogous program
in Java as a proof of concept for the Robo-Rio Marquee
control code.

This program runs from a Linux terminal.

## Logic

The program works as follows:

### Main Program

The program does the following

1. Locate and configure the serial USB connection to the ESP32 Marquee controller,
   whose paths are either
   - '/dev/ttyUSBn'
   - '/dev/ttyACMn'
   where 'n' is a digit
2. Open the USB connection on two file handles, one for read
   and the other for write.
3. Starts a thread that runs the microcontroller monitor in parallel
   with the main logic. The monitor simply reads from the serial
   connection and prints the results on the terminal.
3. Loop forever, sending a sequence of commands to the controller

### FindSerialUsb

The `FindSerialUsb` class constructor implements the class logic. It

1. Scans the /dev/ directory looking for files that match ]
   `/dev/ttyUSB*` and `/dev/ttyACM*`, the serial USB devices
    connected to microcontrollers.
2. Makes the device path available for use by the main
   program.

### UsbStreams

The USBStreams class configures the serial connection, setting speed, parity, character length, and
so on, and then opens two file handles on the device, one for writing (the more important), and the
other for reading. A complete description of UART/Serial I/O settings is way beyond the scope of this
document. Please see the references section for more information.

## Errata

* The `fdatasync` invocation at line 60 in `main.cpp` should probabaly be changed to
  `fsync`.

## References

Please see the following for further information.

* [Serial Port (Wikipedia)](https://en.wikipedia.org/wiki/Serial_port)
* [`termios` manual page](https://en.wikipedia.org/wiki/Serial_port)
* [`getc` manual page](https://linux.die.net/man/3/getc)
* [`fsync` manual page](https://man7.org/linux/man-pages/man2/fdatasync.2.html)
* [`fdatasync` manual page](https://linux.die.net/man/2/fdatasync)
* [`fopen` manual page](https://man7.org/linux/man-pages/man3/fopen.3.html)
* [`opendir` manual page](https://man7.org/linux/man-pages/man3/opendir.3.html)
* [`readdir` manual mage](https://man7.org/linux/man-pages/man3/readdir.3.html)
* [`write` manual page](https://man7.org/linux/man-pages/man2/write.2.html)
* [C++ Input/Output](https://cplusplus.com/reference/iolibrary/)

