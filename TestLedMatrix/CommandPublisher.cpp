/*
 * CommandPublisher.cpp
 *
 *  Created on: Jan 31, 2023
 *      Author: Metuchen Momentum, FRC Robotics Team 7587
 *
 * Parses a raw text command into a DisplayCommand structure and publishes the latter on
 * the command queue.
 */

#include "CommandPublisher.h"
#include <stdlib.h>

static const DisplayCommand command_table[] = {
  ERROR,
  FILL_WITH_COLOR,
  FLASH_TWO_COLORS,
  SINGLE_PIXEL_NATURAL_ORDER,
  SINGLE_PIXEL_LEFT_TO_RIGHT,
  SCROLLING_TEXT,
  STATIC_TEXT,
};

enum CharacterType {
  TEXT,     // Valid non-numeric character
  DIGIT,    // Decimal digit, '0' .. '9';
  PIPE,     // '|'
  CR,
  NL,
  INVALID,  // Invalid character. Raises an error.
  NUMBER_OF_CHARACTER_TYPES,  // MUST be last.
};

/**
 * DFA Transition table. See:
 * https://en.wikipedia.org/wiki/Deterministic_finite_automaton
 */
static const CommandPublisher::STATE
  TRANSITION_TABLE[][NUMBER_OF_CHARACTER_TYPES] = {
    {  // TEXT
      CommandPublisher::STATE::TEXT,      // TEXT
      CommandPublisher::STATE::TEXT,      // DIGIT
      CommandPublisher::STATE::TEXT_END,  // PIPE
      CommandPublisher::STATE::ERROR,     // CR
      CommandPublisher::STATE::ERROR,     // NL
      CommandPublisher::STATE::ERROR,     // INVALID
    },
    {  // TEXT_END
      CommandPublisher::STATE::ERROR,         // TEXT
      CommandPublisher::STATE::COMMAND,       // DIGIT
      CommandPublisher::STATE::COMMAND_END,   // PIPE
      CommandPublisher::STATE::ERROR,         // CR
      CommandPublisher::STATE::ERROR,         // NL
      CommandPublisher::STATE::ERROR,         // INVALID
    },
    {  // COMMAND
      CommandPublisher::STATE::ERROR,         // TEXT
      CommandPublisher::STATE::COMMAND,       // DIGIT
      CommandPublisher::STATE::COMMAND_END,   // PIPE
      CommandPublisher::STATE::ERROR,         // CR
      CommandPublisher::STATE::ERROR,         // NL
      CommandPublisher::STATE::ERROR,         // INVALID
    },
    {  // COMMAND_END
      CommandPublisher::STATE::ERROR,         // TEXT
      CommandPublisher::STATE::DELAY_1,       // DIGIT
      CommandPublisher::STATE::DELAY_1_END,   // PIPE
      CommandPublisher::STATE::ERROR,         // CR
      CommandPublisher::STATE::ERROR,         // NL
      CommandPublisher::STATE::ERROR,		  // INVALID
    },
    {  // DELAY_1
      CommandPublisher::STATE::ERROR,         // TEXT
      CommandPublisher::STATE::DELAY_1,       // DIGIT
      CommandPublisher::STATE::DELAY_1_END,   // PIPE
      CommandPublisher::STATE::ERROR,         // CR
      CommandPublisher::STATE::ERROR,         // NL
      CommandPublisher::STATE::ERROR,		  // INVALID
    },
    {  // DELAY_1_END
      CommandPublisher::STATE::ERROR,         // TEXT
      CommandPublisher::STATE::DELAY_2,       // DIGIT
      CommandPublisher::STATE::DELAY_2_END,   // PIPE
      CommandPublisher::STATE::ERROR,         // CR
      CommandPublisher::STATE::ERROR,         // NL
      CommandPublisher::STATE::ERROR,		  // INVALID
    },
    {  // DELAY_2
      CommandPublisher::STATE::ERROR,         // TEXT
      CommandPublisher::STATE::DELAY_2,       // DIGIT
      CommandPublisher::STATE::DELAY_2_END,   // PIPE
      CommandPublisher::STATE::ERROR,         // CR
      CommandPublisher::STATE::ERROR,         // NL
      CommandPublisher::STATE::ERROR,		  // INVALID
    },
    {  // DELAY_2_END
      CommandPublisher::STATE::ERROR,               // TEXT
      CommandPublisher::STATE::FOREGROUND_RED,      // DIGIT
      CommandPublisher::STATE::FOREGROUND_RED_END,  // PIPE
      CommandPublisher::STATE::ERROR,               // CR
      CommandPublisher::STATE::ERROR,               // NL
      CommandPublisher::STATE::ERROR,		        // INVALID
    },
    {  // FOREGROUND_RED
      CommandPublisher::STATE::ERROR,               // TEXT
      CommandPublisher::STATE::FOREGROUND_RED,      // DIGIT
      CommandPublisher::STATE::FOREGROUND_RED_END,  // PIPE
      CommandPublisher::STATE::ERROR,               // CR
      CommandPublisher::STATE::ERROR,               // NL
      CommandPublisher::STATE::ERROR,		        // INVALID
    },
    {  // FOREGROUND_RED_END
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::FOREGROUND_GREEN,      // DIGIT
      CommandPublisher::STATE::FOREGROUND_GREEN_END,  // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // FOREGROUND_GREEN
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::FOREGROUND_GREEN,      // DIGIT
      CommandPublisher::STATE::FOREGROUND_GREEN_END,  // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // FOREGROUND_GREEN_END
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::FOREGROUND_BLUE,       // DIGIT
      CommandPublisher::STATE::FOREGROUND_BLUE_END,   // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // FOREGROUND_BLUE
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::FOREGROUND_BLUE,       // DIGIT
      CommandPublisher::STATE::FOREGROUND_BLUE_END,   // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // FOREGROUND_BLUE_END
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::BACKGROUND_RED,        // DIGIT
      CommandPublisher::STATE::BACKGROUND_RED_END,    // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // BACKGROUND_RED
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::BACKGROUND_RED,        // DIGIT
      CommandPublisher::STATE::BACKGROUND_RED_END,    // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // BACKGROUND_RED_END
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::BACKGROUND_GREEN,      // DIGIT
      CommandPublisher::STATE::BACKGROUND_GREEN_END,  // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // BACKGROUND_GREEN
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::BACKGROUND_GREEN,      // DIGIT
      CommandPublisher::STATE::BACKGROUND_GREEN_END,  // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // BACKGROUND_GREEN_END
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::BACKGROUND_BLUE,       // DIGIT
      CommandPublisher::STATE::ERROR,                 // PIPE
      CommandPublisher::STATE::IGNORING_RETURN,       // CR
      CommandPublisher::STATE::END_OF_COMMAND,        // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // BACKGROUND_BLUE
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::BACKGROUND_BLUE,       // DIGIT
      CommandPublisher::STATE::ERROR,                 // PIPE
      CommandPublisher::STATE::IGNORING_RETURN,       // CR
      CommandPublisher::STATE::END_OF_COMMAND,        // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // IGNORING RETURN
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::ERROR,                 // DIGIT
      CommandPublisher::STATE::ERROR,                 // PIPE
      CommandPublisher::STATE::IGNORING_RETURN,       // CR
      CommandPublisher::STATE::END_OF_COMMAND,        // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // END_OF_COMMAND -- code will reset state to TEXT
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::ERROR,                 // DIGIT
      CommandPublisher::STATE::ERROR,                 // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // ERROR
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::ERROR,                 // DIGIT
      CommandPublisher::STATE::ERROR,                 // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::END_OF_ERROR,          // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
    {  // END_OF_ERROR -- code will reset state to TEXT
      CommandPublisher::STATE::ERROR,                 // TEXT
      CommandPublisher::STATE::ERROR,                 // DIGIT
      CommandPublisher::STATE::ERROR,                 // PIPE
      CommandPublisher::STATE::ERROR,                 // CR
      CommandPublisher::STATE::ERROR,                 // NL
      CommandPublisher::STATE::ERROR,		          // INVALID
    },
};


static CharacterType character_type_of(int incoming_byte) {
  CharacterType character_type = CharacterType::INVALID;
  if (' ' <= incoming_byte && incoming_byte <= '~'
      || incoming_byte == '\n'
      || incoming_byte == '\r') {
    switch (incoming_byte) {
    case '|':
      character_type = PIPE;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      character_type = DIGIT;
      break;
    case '\n':
      character_type = NL;
      break;
    case '\r':
      character_type = CR;
      break;
    default:
      character_type = TEXT;
    }
  }

  return character_type;
}

static const unsigned char * INVALID_COMMAND =
    (const unsigned char *) "BOGUS";

static BaseType_t add_digit(BaseType_t value, unsigned char digit) {
  return value * 10 + (digit - '0');
}

void CommandPublisher::init() {
  memset(&command_buffer, 0, sizeof(command_buffer));
  memset(&command_to_publish, 0, sizeof(command_to_publish));
}

CommandPublisher::CommandPublisher() :
  h_command_queue(NULL) {
  init();
}

CommandPublisher::~CommandPublisher() {
}

void CommandPublisher::begin(QueueHandle_t h_command_queue) {
  this->h_command_queue = h_command_queue;
}

void CommandPublisher::parse_and_publish(const unsigned char * raw_command) {
  init();
  STATE state = TEXT;
  int16_t number_of_characters = 0;
  int16_t command_index = 0;
  BaseType_t delay_1 = 0;
  BaseType_t delay_2 = 0;
  BaseType_t foreground_red = 0;
  BaseType_t foreground_green = 0;
  BaseType_t foreground_blue = 0;
  BaseType_t background_red = 0;
  BaseType_t background_green = 0;
  BaseType_t background_blue = 0;
  const unsigned char *text_to_publish = command_buffer;
  unsigned char incoming_byte = '\0';

  while ((number_of_characters < MAX_TEXT_LENGTH - 1)
      && (incoming_byte = *(raw_command++))
      && (state != END_OF_COMMAND)
      && (state != END_OF_ERROR)) {
    CharacterType character_type = character_type_of((int) incoming_byte);
    state = TRANSITION_TABLE[state][character_type];

    unsigned char incoming_char = (unsigned char) incoming_byte;
    switch (state) {
      case TEXT:
        command_buffer[number_of_characters++] = incoming_char;
        command_to_publish.text_length = number_of_characters;
        break;

      case TEXT_END:
        // Nothing to do.
        break;

      case COMMAND:
        command_index = add_digit(command_index, incoming_char);
        break;

      case COMMAND_END:
        // Nothing to do.
        break;

      case DELAY_1:
        delay_1 = add_digit(delay_1, incoming_char);
        break;

      case DELAY_1_END:
        // Nothing to do.
        break;

      case DELAY_2:
        delay_2 = add_digit(delay_2, incoming_char);
        break;

      case DELAY_2_END:
        // Nothing to do.
        break;

      case FOREGROUND_RED:
        foreground_red = add_digit(foreground_red, incoming_char);
        break;

      case FOREGROUND_RED_END:
        // Nothing to do.
        break;

      case FOREGROUND_GREEN:
        foreground_green =
            add_digit(foreground_green, incoming_char);
        break;

      case FOREGROUND_GREEN_END:
        // Nothing to do.
        break;

      case FOREGROUND_BLUE:
        foreground_blue =
            add_digit(foreground_blue, incoming_char);
        break;

      case FOREGROUND_BLUE_END:
        // Nothing to do.
        break;

      case BACKGROUND_RED:
        background_red =
        add_digit(background_red, incoming_char);

        break;

      case BACKGROUND_RED_END:
        // Nothing to do.
        break;

      case BACKGROUND_GREEN:
        background_green =
        add_digit(background_green, incoming_char);
        break;

      case BACKGROUND_GREEN_END:
        // Nothing to do
        break;

      case BACKGROUND_BLUE:
        background_blue =
        add_digit(background_blue, incoming_char);
        break;

      case IGNORING_RETURN:
        // Nothing to do.
        break;

      case END_OF_COMMAND:
        Serial.println("Valid command.");
        break;

      case ERROR:
        // Nothing to do.
        break;

      case END_OF_ERROR:
        text_to_publish = INVALID_COMMAND;
        command_to_publish.text_length = 5;
        command_index = 0;
        Serial.println("Bad command.");
        break;
    }
  }
  //	print_command_to_publish(command_to_publish);
  state = TEXT;
  number_of_characters = 0;
  command_to_publish.p_text = text_to_publish;
  // command_to_publish.text_length is already set.
  command_to_publish.command =
    0 <= command_index
    && command_index < NUMBER_OF_COMMANDS
      ? command_table[command_index]
      : command_table[0];
  command_to_publish.delay1 = delay_1;
  command_to_publish.delay2 = delay_2;
  command_to_publish.foreground.red = foreground_red;
  command_to_publish.foreground.green = foreground_green;
  command_to_publish.foreground.blue = foreground_blue;
  command_to_publish.background.red = background_red;
  command_to_publish.background.green = background_green;
  command_to_publish.background.blue = background_blue;
  xQueueSendToBack(h_command_queue, &command_to_publish, pdMS_TO_TICKS(25));
  Serial.println("Command sent");
}

void CommandPublisher::publish_error() {
  memset(&command_to_publish, 0, sizeof(command_to_publish));
  command_to_publish.command = DisplayCommand::ERROR;
  xQueueSendToBack(
    h_command_queue, &command_to_publish, pdMS_TO_TICKS(25));
}

