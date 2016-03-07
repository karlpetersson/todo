#include "command.h"
#include <string.h>

void cmd_new(Command_t *cmd, size_t data_length) {
	cmd->data = malloc(data_length * sizeof(char));
	memset(cmd->data, 0, data_length);
}

void cmd_free(Command_t *cmd) {
	free(cmd->data);
}