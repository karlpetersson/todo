#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "command.h"

static const int MAX_LENGTH_DATA = 1024;

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
	cmd_new(&cmd, MAX_LENGTH_DATA);
	if (argc <= 1) {
		cmd.type = COMMAND_LIST;
	}
	else if(argc < 3) {
		cmd.type = COMMAND_INVALID;
		strcat(cmd.data, "Invalid number of arguments");
		//message about why invalid, num arguments
	} else {
		if(strcmp(argv[1], "add") == 0 ||
				strcmp(argv[1], "a") == 0) {
			int i;
			cmd.type = COMMAND_ADD;
			for(i = 2; i < argc; i++) {
				strcat(cmd.data, argv[i]);
				if(i != argc) strcat(cmd.data, " ");
			} 

		} else if(strcmp(argv[1], "do") == 0 ||
				strcmp(argv[1], "d") == 0) {
			cmd.type = COMMAND_FINISH;
			sanitize_num_range(argv[2], &cmd.data[0]);

		} else if(strcmp(argv[1], "prio") == 0 ||
				strcmp(argv[1], "p") == 0) {
			cmd.type = COMMAND_PRIO;
			sanitize_num_range(argv[2], &cmd.data[0]);
			// add data for prio

		} else if(strcmp(argv[1], "load") == 0 ||
				strcmp(argv[1], "l") == 0) {
			// use getopt for this
			cmd.type = COMMAND_LOAD;
		}
	}

	return cmd;
}

