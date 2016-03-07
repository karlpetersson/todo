#include <stdlib.h>

typedef enum {
	COMMAND_INVALID,
	COMMAND_LIST,
	COMMAND_ADD,
	COMMAND_FINISH,
	COMMAND_PRIO,
	COMMAND_LOAD
} command_type_t;

typedef struct {
	command_type_t type;
	char *data;
} Command_t; 

void cmd_new(Command_t *cmd, size_t data_length);
void cmd_free(Command_t *cmd);