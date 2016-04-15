#include "command.h"
#include <string.h>

void cmd_new(Command_t *cmd, size_t data_size) {
	cmd->data = malloc(data_size);
	memset(cmd->data, 0, data_size);
}

void cmd_free(Command_t *cmd) {
	free(cmd->data);
}