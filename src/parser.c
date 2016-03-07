#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "command.h"

static const int MAX_LENGTH_DATA = 1024;

//TODO: unused
static void sanitize_num_range(char *in, char *out) {
	char *buf;
	char delim[2];
	char *p;
	int low, hi, count;
	delim[1] = 0;

	if((p = strtok(in, ",")) != NULL) {
		buf = strdup(p);
		strcat(out, buf);
	}

	while((p = strtok(NULL, ",")) != NULL) {
		buf = strdup(p);
		strcat(out, ",");
		strcat(out, buf);
	}
}

//find correct command, sanitize input
Command_t parser_command(int argc, char **argv) {
	Command_t cmd;

	if (argc <= 1) {
		cmd.type = COMMAND_LIST;
		cmd.data = NULL;
	} else if(argc < 3) {
		cmd.type = COMMAND_INVALID;
		cmd_new(&cmd, sizeof(char), MAX_LENGTH_DATA);
		strcat(cmd.data, "Invalid number of arguments");
	} else {
		if(strcmp(argv[1], "add") == 0 ||
				strcmp(argv[1], "a") == 0) {
			cmd.type = COMMAND_ADD;
			cmd_new(&cmd, sizeof(char), MAX_LENGTH_DATA);			
			
			for(int i = 2; i < argc; i++) {
				strcat(cmd.data, argv[i]);
				if(i != argc) strcat(cmd.data, " ");
			} 

		} else if(strcmp(argv[1], "do") == 0 ||
				strcmp(argv[1], "d") == 0) {
			int p = atoi(argv[2]);
			cmd.type = COMMAND_FINISH;
			cmd_new(&cmd, sizeof(int), 1);
			memcpy(cmd.data, &p, sizeof(int));
			//sanitize_num_range(argv[2], &cmd.data[0]);

		} else if(strcmp(argv[1], "prio") == 0 ||
				strcmp(argv[1], "p") == 0) {
			int p = atoi(argv[2]);
			cmd.type = COMMAND_PRIO;
			cmd_new(&cmd, sizeof(int), 1);
			memcpy(cmd.data, &p, sizeof(int));
			//sanitize_num_range(argv[2], &cmd.data[0]);
			// add data for prio

		} else if(strcmp(argv[1], "load") == 0 ||
				strcmp(argv[1], "l") == 0) {
			// use getopt for this
			cmd.type = COMMAND_LOAD;
		}
	}

	return cmd;
}

