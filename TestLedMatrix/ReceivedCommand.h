/*
 * ReceivedCommand.h
 *
 *  Created on: Nov 26, 2022
 *      Author: Eric Mintz
 *
 * Queue entry for a received Marquee command.
 */

#ifndef RECEIVEDCOMMAND_H_
#define RECEIVEDCOMMAND_H_

#define MAX_COMMAND_LENGTH 128

typedef struct {
	char command[MAX_COMMAND_LENGTH];
} ReceivedCommand;

#endif /* RECEIVEDCOMMAND_H_ */
