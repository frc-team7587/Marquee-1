/*
 * SerialReadTask.cpp
 *
 *  Created on: July 30, 2022
 *      Author: Eric Mintz
 */

#include "SerialReadTask.h"
#include "DisplayCommand.h"
#include "DisplayMessage.h"

static const DisplayCommand command_table[] = {
	ERROR,
	FILL_WITH_COLOR,
	FLASH_TWO_COLORS,
	SINGLE_PIXEL_NATURAL_ORDER,
	SINGLE_PIXEL_LEFT_TO_RIGHT,
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
static const SerialReadTask::STATE
	TRANSITION_TABLE[][NUMBER_OF_CHARACTER_TYPES] = {
		{  // TEXT
			SerialReadTask::STATE::TEXT,      // TEXT
			SerialReadTask::STATE::TEXT,      // DIGIT
			SerialReadTask::STATE::TEXT_END,  // PIPE
			SerialReadTask::STATE::ERROR,     // CR
			SerialReadTask::STATE::ERROR,     // NL
			SerialReadTask::STATE::ERROR,     // INVALID
		},
		{  // TEXT_END
			SerialReadTask::STATE::ERROR,         // TEXT
			SerialReadTask::STATE::COMMAND,       // DIGIT
			SerialReadTask::STATE::COMMAND_END,   // PIPE
			SerialReadTask::STATE::ERROR,         // CR
			SerialReadTask::STATE::ERROR,         // NL
			SerialReadTask::STATE::ERROR,         // INVALID
		},
		{  // COMMAND
			SerialReadTask::STATE::ERROR,         // TEXT
			SerialReadTask::STATE::COMMAND,       // DIGIT
			SerialReadTask::STATE::COMMAND_END,   // PIPE
			SerialReadTask::STATE::ERROR,         // CR
			SerialReadTask::STATE::ERROR,         // NL
			SerialReadTask::STATE::ERROR,         // INVALID
		},
		{  // COMMAND_END
			SerialReadTask::STATE::ERROR,         // TEXT
			SerialReadTask::STATE::DELAY_1,       // DIGIT
			SerialReadTask::STATE::DELAY_1_END,   // PIPE
			SerialReadTask::STATE::ERROR,         // CR
			SerialReadTask::STATE::ERROR,         // NL
			SerialReadTask::STATE::ERROR,		  // INVALID
		},
		{  // DELAY_1
			SerialReadTask::STATE::ERROR,         // TEXT
			SerialReadTask::STATE::DELAY_1,       // DIGIT
			SerialReadTask::STATE::DELAY_1_END,   // PIPE
			SerialReadTask::STATE::ERROR,         // CR
			SerialReadTask::STATE::ERROR,         // NL
			SerialReadTask::STATE::ERROR,		  // INVALID
		},
		{  // DELAY_1_END
			SerialReadTask::STATE::ERROR,         // TEXT
			SerialReadTask::STATE::DELAY_2,       // DIGIT
			SerialReadTask::STATE::DELAY_2_END,   // PIPE
			SerialReadTask::STATE::ERROR,         // CR
			SerialReadTask::STATE::ERROR,         // NL
			SerialReadTask::STATE::ERROR,		  // INVALID
		},
		{  // DELAY_2
			SerialReadTask::STATE::ERROR,         // TEXT
			SerialReadTask::STATE::DELAY_2,       // DIGIT
			SerialReadTask::STATE::DELAY_2_END,   // PIPE
			SerialReadTask::STATE::ERROR,         // CR
			SerialReadTask::STATE::ERROR,         // NL
			SerialReadTask::STATE::ERROR,		  // INVALID
		},
		{  // DELAY_2_END
			SerialReadTask::STATE::ERROR,               // TEXT
			SerialReadTask::STATE::FOREGROUND_RED,      // DIGIT
			SerialReadTask::STATE::FOREGROUND_RED_END,  // PIPE
			SerialReadTask::STATE::ERROR,               // CR
			SerialReadTask::STATE::ERROR,               // NL
			SerialReadTask::STATE::ERROR,		        // INVALID
		},
		{  // FOREGROUND_RED
			SerialReadTask::STATE::ERROR,               // TEXT
			SerialReadTask::STATE::FOREGROUND_RED,      // DIGIT
			SerialReadTask::STATE::FOREGROUND_RED_END,  // PIPE
			SerialReadTask::STATE::ERROR,               // CR
			SerialReadTask::STATE::ERROR,               // NL
			SerialReadTask::STATE::ERROR,		        // INVALID
		},
		{  // FOREGROUND_RED_END
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::FOREGROUND_GREEN,      // DIGIT
			SerialReadTask::STATE::FOREGROUND_GREEN_END,  // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // FOREGROUND_GREEN
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::FOREGROUND_GREEN,      // DIGIT
			SerialReadTask::STATE::FOREGROUND_GREEN_END,  // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // FOREGROUND_GREEN_END
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::FOREGROUND_BLUE,       // DIGIT
			SerialReadTask::STATE::FOREGROUND_BLUE_END,   // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // FOREGROUND_BLUE
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::FOREGROUND_BLUE,       // DIGIT
			SerialReadTask::STATE::FOREGROUND_BLUE_END,   // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // FOREGROUND_BLUE_END
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::BACKGROUND_RED,        // DIGIT
			SerialReadTask::STATE::BACKGROUND_RED_END,    // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // BACKGROUND_RED
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::BACKGROUND_RED,        // DIGIT
			SerialReadTask::STATE::BACKGROUND_RED_END,    // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // BACKGROUND_RED_END
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::BACKGROUND_GREEN,      // DIGIT
			SerialReadTask::STATE::BACKGROUND_GREEN_END,  // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // BACKGROUND_GREEN
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::BACKGROUND_GREEN,      // DIGIT
			SerialReadTask::STATE::BACKGROUND_GREEN_END,  // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // BACKGROUND_GREEN_END
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::BACKGROUND_BLUE,       // DIGIT
			SerialReadTask::STATE::ERROR,                 // PIPE
			SerialReadTask::STATE::IGNORING_RETURN,       // CR
			SerialReadTask::STATE::END_OF_COMMAND,        // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // BACKGROUND_BLUE
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::BACKGROUND_BLUE,       // DIGIT
			SerialReadTask::STATE::ERROR,                 // PIPE
			SerialReadTask::STATE::IGNORING_RETURN,       // CR
			SerialReadTask::STATE::END_OF_COMMAND,        // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // IGNORING RETURN
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::ERROR,                 // DIGIT
			SerialReadTask::STATE::ERROR,                 // PIPE
			SerialReadTask::STATE::IGNORING_RETURN,       // CR
			SerialReadTask::STATE::END_OF_COMMAND,        // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // END_OF_COMMAND -- code will reset state to TEXT
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::ERROR,                 // DIGIT
			SerialReadTask::STATE::ERROR,                 // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // ERROR
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::ERROR,                 // DIGIT
			SerialReadTask::STATE::ERROR,                 // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::END_OF_ERROR,          // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
		{  // END_OF_ERROR -- code will reset state to TEXT
			SerialReadTask::STATE::ERROR,                 // TEXT
			SerialReadTask::STATE::ERROR,                 // DIGIT
			SerialReadTask::STATE::ERROR,                 // PIPE
			SerialReadTask::STATE::ERROR,                 // CR
			SerialReadTask::STATE::ERROR,                 // NL
			SerialReadTask::STATE::ERROR,		          // INVALID
		},
};

static const unsigned char * INVALID_COMMAND =
		(const unsigned char *) "BOGUS";

static unsigned char display_buffer[256];

static void print_message(const DisplayMessage &message) {
	memset(display_buffer, '\0', 256);
	memcpy(display_buffer, message.p_text, message.text_length);
	Serial.print("Command: ");
	Serial.println(message.command);
	Serial.print("Text: ");
	Serial.println((const char *)display_buffer);
	Serial.print("Foreground: <");
	Serial.print(message.foreground.red);
	Serial.print(", ");
	Serial.print(message.foreground.green);
	Serial.print(", ");
	Serial.print(message.foreground.blue);
	Serial.print("> Background: <");
	Serial.print(message.background.red);
	Serial.print(", ");
	Serial.print(message.background.green);
	Serial.print(", ");
	Serial.print(message.background.blue);
	Serial.println(">");

	Serial.print("Delay 1: ");
	Serial.println(message.delay1);
	Serial.print("Delay 2: ");
	Serial.println(message.delay2);
}

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

static BaseType_t add_digit(BaseType_t value, unsigned char digit) {
	return value * 10 + (digit - '0');
}

SerialReadTask::SerialReadTask() {
	reset();
}

SerialReadTask::~SerialReadTask() {
}

void SerialReadTask::run(QueueHandle_t h_command_queue) {
	Serial.println("Serial read task started.");
	DisplayMessage message;
	memset(display_buffer, '\0', 256);
	unsigned char *p_display_buffer = display_buffer;
	for (;;) {
		while (Serial.available()) {
			int incoming_byte = Serial.read();
			CharacterType character_type = character_type_of(incoming_byte);
			state = TRANSITION_TABLE[state][character_type];

			unsigned char incoming_char = (unsigned char) incoming_byte;
//			*p_display_buffer++ = incoming_char;
//			Serial.print("State: ");
//			Serial.print(state);
//			Serial.print(", character type: ");
//			Serial.print(character_type);
//			Serial.print(" >>");
//			Serial.print((const char *)display_buffer);
//			Serial.println("<<");

			switch (state) {
				case TEXT:
					if (number_of_characters >= sizeof(command_buffer)) {
						state = ERROR;
					} else {
						command_buffer[number_of_characters++] = incoming_char;
						message.text_length = number_of_characters;
					}
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
					// TODO: Send command.
					state = TEXT;
					number_of_characters = 0;
					Serial.println("Valid command.");
					message.p_text = command_buffer;
					// message.text_length is already set.
					message.command =
						0 <= command_index
						&& command_index < NUMBER_OF_COMMANDS
							? command_table[command_index]
							: command_table[0];
					message.delay1 = delay_1;
					message.delay2 = delay_2;
					message.foreground.red = foreground_red;
					message.foreground.green = foreground_green;
					message.foreground.blue = foreground_blue;
					message.background.red = background_red;
					message.background.green = background_green;
					message.background.blue = background_blue;
//					print_message(message);
					p_display_buffer = display_buffer;
					memset(display_buffer, '\0', 256);
					xQueueSendToBack(
						h_command_queue, &message, pdMS_TO_TICKS(25));
					reset();
					break;
				case ERROR:
					// Nothing to do.
					break;
				case END_OF_ERROR:
					message.p_text = INVALID_COMMAND;
					message.text_length = 5;
					message.command = DisplayCommand::ERROR;
					message.delay1 = 0;
					message.delay2 = 0;
					message.foreground = CRGB::Red;
					message.background = CRGB::Black;
					state = TEXT;
					number_of_characters = 0;
					Serial.println("Bad command.");
//					print_message(message);
					p_display_buffer = display_buffer;
					memset(display_buffer, '\0', 256);
					xQueueSendToBack(
						h_command_queue, &message, pdMS_TO_TICKS(25));
					reset();
					break;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void SerialReadTask::reset() {
	state = TEXT;
	number_of_characters = 0;
	command_index = 0;
	delay_1 = 0;
	delay_2 = 0;
	foreground_red = 0;
	foreground_green = 0;
	foreground_blue = 0;
	background_red = 0;
	background_green = 0;
	background_blue = 0;
}
