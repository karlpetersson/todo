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

	todolist_create(&tl);
	tl.file = fp;

	todolist_from_file(&tl);
	fclose(fp);
	// parse input cmd

	cmd = parser_command(argc, argv);
	printf("%s\n", cmd.data);
	
	switch(cmd.type) {
		case COMMAND_INVALID:
			break;
		case COMMAND_LIST:
			todolist_print(&tl);
			break;
		case COMMAND_ADD:
			todolist_add(&tl, (char *)cmd.data);
			break;
		case COMMAND_FINISH:
			todolist_finish(&tl, (int *)cmd.data);
			//strtok(cmd.data, ",");
			break;
		case COMMAND_PRIO:
			//todolist_prio(&tl, cmd.data);
			break;
		case COMMAND_LOAD:
			//todolist_load(&tl, cmd.data);
			break;
	}

	if(!(fp = fopen(path, "w"))) {
		fprintf(stderr, "sry cant open file\n");
		exit(EXIT_FAILURE);
	}
	
	cmd_free(&cmd);
	todolist_save(&tl);
	//todolist_destroy(&tl);
}

