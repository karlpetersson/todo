#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "todo.h"
#include "command.h"
#include "parser.h"
#include "keyhandler.h"
#include "terminal.h"

const char *path = "./todo.txt";

static void interactive_mode(TodoList_t *tlist) {
	Key_t pressed_key = 0;
	int cursor_pos = 1;

	if(0 != term_raw_mode()) {
		printf("raw term fail");
		exit(EXIT_FAILURE);
	}

	while(pressed_key != KEY_ESC) {
		TerminalState_t tstate;
		term_init(&tstate);

		todolist_render(tlist, tstate.writebuf, cursor_pos);

		term_render(&tstate);

		term_wait_for_key(&tstate);

		pressed_key = key_from_raw_input(tstate.readbuf);

		switch(pressed_key) {
			case KEY_UP:
				if(--cursor_pos < 1) {
					cursor_pos = 1;
				}
				break;
			case KEY_DOWN:
				if(++cursor_pos > todolist_length(tlist)) {
					cursor_pos = todolist_length(tlist);
				}
				break;
			case KEY_A:
				term_wait_for_text(&tstate);
				todolist_add(tlist, tstate.readbuf, 0);
				break;
			case KEY_D:
				todolist_finish(tlist, &cursor_pos);
				break;
			case KEY_P:
				todolist_set_priority(tlist, &cursor_pos, &cursor_pos);
				break;	
			case KEY_ENTER:
				break;
			case KEY_ESC:
				break; //exit program
			default:
				break;
		}
	}

	if(0 != term_norm_mode()) {
		printf("fail reset term mode");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	FILE *fp;
	TodoList_t tl;
	Command_t cmd;

	todolist_create(&tl);
	tl.file = fp;

	if(!todolist_from_file(&tl, path)) {
		fprintf(stderr, "couldnt open file =(\n");
		exit(EXIT_FAILURE);
	}

	// parse input cmd
	cmd = parser_command(argc, argv);
	
	switch(cmd.type) {
		case COMMAND_INVALID:
			printf("%s\n", cmd.data);
			break;
		case COMMAND_LIST:
			todolist_print(&tl);
			break;
		case COMMAND_ADD:
			todolist_add(&tl, (char *)cmd.data, 0);
			todolist_print(&tl);
			break;
		case COMMAND_FINISH:
			todolist_finish(&tl, (int *)cmd.data);
			todolist_print(&tl);
			break;
		case COMMAND_PRIO:
			todolist_set_priority(&tl, &((int *)cmd.data)[0], &((int *)cmd.data)[1]);
			todolist_print(&tl);
			break;
		case COMMAND_LOAD:
			//todolist_load(&tl, cmd.data);
			break;
		case COMMAND_INTERACTIVE:
			interactive_mode(&tl);
			break;
		default:
			break;
	}
	
	if(!todolist_save(&tl, path)) {
		fprintf(stderr, "sry cant open file\n");
		exit(EXIT_FAILURE);
	}

	//todolist_destroy();
	cmd_free(&cmd);
}

