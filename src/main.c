#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "todo.h"
#include "command.h"
#include "parser.h"

const char *path = "./todo.txt";

int main(int argc, char **argv) {
	FILE *fp;
	TodoList_t tl;
	Command_t cmd;

	if(!(fp = fopen(path, "r"))) {
		fprintf(stderr, "sry cant open file\n");
		exit(EXIT_FAILURE);
	}
	printf("file opened.\n");

	tl.file = fp;
	todo_parse_file(&tl);
	fclose(fp);
	// parse input cmd

	cmd = parser_command(argc, argv);
	printf("%s\n", cmd.data);

	switch(cmd.type) {
		case COMMAND_INVALID:
			break;
		case COMMAND_LIST:
			todo_print(&tl);
			break;
		case COMMAND_ADD:
			todo_add(&tl, cmd.data);
			break;
		case COMMAND_FINISH:
			break;
		case COMMAND_PRIO:
			break;
		case COMMAND_LOAD:
			break;
	}

	if(!(fp = fopen(path, "w"))) {
		fprintf(stderr, "sry cant open file\n");
		exit(EXIT_FAILURE);
	}
	
	cmd_free(&cmd);
	todo_save(&tl);
	todo_destroy(&tl);

}