#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "command.h"

#define PARSER_ERR "Parser: "
#define PARSER_ERR_NUM_ARGS "Parser: Too few arguments to command "
#define PARSER_ERR_NOT_NUM "Parser: Command argument needs to be a number "

static const int BASE10 = 10;
static const int MAX_LENGTH_DATA = 1024;
static const int MAX_ERROR_MSG_LENGTH = 512;

static void parser_error(Command_t *cmd, char const *msg) {
	cmd_new(cmd, sizeof(char), MAX_ERROR_MSG_LENGTH);
	cmd->type = COMMAND_INVALID;
	strncat(cmd->data, msg, MAX_ERROR_MSG_LENGTH);
}

//find correct command, sanitize input
Command_t parser_command(int argc, char **argv) {
	Command_t cmd;

	/* LIST */
	if (argc <= 1) {
		cmd_new(&cmd, 0, 0);
		cmd.type = COMMAND_LIST;
		cmd.data = NULL;
	} else {

		/* ADD */
		if(strcmp(argv[1], "add") == 0 || strcmp(argv[1], "a") == 0) {
			if(argc <= 2) {
				parser_error(&cmd, PARSER_ERR_NUM_ARGS "(add)");
			} else {
				cmd_new(&cmd, sizeof(char), MAX_LENGTH_DATA);	
				cmd.type = COMMAND_ADD;
		
				//TODO: check for too long strings
				for(int i = 2; i < argc; i++) {
					strncat(cmd.data, argv[i], MAX_LENGTH_DATA);
					if(i != argc) strncat(cmd.data, " ", MAX_LENGTH_DATA);
				} 
			}

		/* DO */
		} else if(strcmp(argv[1], "do") == 0 || strcmp(argv[1], "d") == 0) {
			int p;
			char *end;
			if(argc <= 2) {
				parser_error(&cmd, PARSER_ERR_NUM_ARGS "(do)");
			} else {	
				p = strtol(argv[2], &end, BASE10);

				if(*end) {
					parser_error(&cmd, PARSER_ERR_NOT_NUM "(do)");
				} else {
					cmd_new(&cmd, sizeof(int), 1);
					cmd.type = COMMAND_FINISH;
					memcpy(cmd.data, &p, sizeof(int));
				}		
			}

		/* PRIO */
		} else if(strcmp(argv[1], "prio") == 0 || strcmp(argv[1], "p") == 0) {
			int p[2];
			char *end1, *end2;

			if(argc <= 3) {
				parser_error(&cmd, PARSER_ERR_NUM_ARGS "(prio)");
			} else {
				p[0] = strtol(argv[2], &end1, BASE10);
				p[1] = strtol(argv[3], &end2, BASE10);

				if(*end1 || *end2) {
					parser_error(&cmd, PARSER_ERR_NOT_NUM "(prio)");
				} else {
					cmd_new(&cmd, sizeof(int), 2);
					cmd.type = COMMAND_PRIO;				
					memcpy(cmd.data, &p, sizeof(int) * 2);
				}
			}
		/* LOAD */
		} else if(strcmp(argv[1], "load") == 0 || strcmp(argv[1], "l") == 0) {
			// use getopt for this
			cmd.type = COMMAND_LOAD;
		} else if(strcmp(argv[1], "i") == 0) {
			cmd_new(&cmd, 0, 0);
			cmd.type = COMMAND_INTERACTIVE;
		} else {
			parser_error(&cmd, PARSER_ERR "Unknown command");
		}
	}

	return cmd;
}

