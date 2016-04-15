#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>

typedef enum {
	COMMAND_INVALID,
	COMMAND_INIT,
	COMMAND_LIST,
	COMMAND_ADD,
	COMMAND_FINISH,
	COMMAND_PRIO,
	COMMAND_LOAD,
	COMMAND_INTERACTIVE,
	NUM_COMMANDS
} command_type_t;

typedef struct {
	command_type_t type;
	void *data;
} Command_t; 

void cmd_new(Command_t *cmd, size_t data_size);
void cmd_free(Command_t *cmd);

#endif