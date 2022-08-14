/*
 * PwmPinsAndChannels.h
 *
 *  Created on: June 12, 2022
 *      Author: Eric Mintz
 *
 *  Pins and channel configuration for the PWM and LED string signals
 *  Note that the assignment includes pins for an accelerometer that
 *  is not part of this prototype.
 */

#ifndef PWMPINSANDCHANNELS_H_
#define PWMPINSANDCHANNELS_H_

// PWM settings
#define PWM_FREQUENCY 512
#define PWM_RESOLUTION 10  // 0 .. 1023

// Acceleration pins:
#define TILT_BLUE_PIN  16
#define TILT_GREEN_PIN  4
#define TILT_RED_PIN    0

// Acceleration PWM channels:
#define TILT_BLUE_CHANNEL 0
#define TILT_GREEN_CHANNEL 1
#define TILT_RED_CHANNEL 2

// Tilt pins:
#define SHOCK_BLUE_PIN  19
#define SHOCK_GREEN_PIN 18
#define SHOCK_RED_PIN   17

// Tilt PWM Channels
#define SHOCK_BLUE_CHANNEL  3
#define SHOCK_GREEN_CHANNEL 4
#define SHOCK_RED_CHANNEL   5

// LED string (or array) pins
#define LED_ARRAY_PIN 15

#endif /* PWMPINSANDCHANNELS_H_ */
