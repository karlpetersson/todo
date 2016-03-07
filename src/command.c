#include "command.h"
#include <string.h>

void cmd_new(Command_t *cmd, size_t data_size, int length) {
	cmd->data = malloc(data_size * length);
	memset(cmd->data, 0, length);
}

void cmd_free(Command_t *cmd) {
	free(cmd->data);
}