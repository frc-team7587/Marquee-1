# Serial to Wire (I2C) Command Forwarder

An ESP32 program that receives text commands from the USB Serial
Input (a.k.a. Serial) and sends it to a slave over I2C. Note
that the slave address is hard wired, and that the program
uses 7 bit I2C addresses.


