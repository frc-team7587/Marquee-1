/*
 * DisplayMessage.cpp
 *
 *  Created on: July 17, 2022
 *      Author: Eric Mintz
 */

#include "DisplayMessage.h"

DisplayMessage::DisplayMessage()  :
  p_text(0),
  text_length(0),
  delay1(0),
  delay2(0),
  command(),
  foreground(CRGB::White),
  background(CRGB::Black) {
}

const DisplayMessage& DisplayMessage::operator=(const DisplayMessage& rhs) {
  p_text = rhs.p_text;
  text_length = rhs.text_length;
  command = rhs.command;
  foreground = rhs.foreground;
  background = rhs.background;
  return *this;
}
