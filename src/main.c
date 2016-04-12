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
#include "concat.h"

#define GREP_PATTERN "TODO:"
#define INPUT_TEXT_SIZE (1024)
#define INPUT_KEY_SIZE (64)

const char *todo_path = "./todo.txt";
const char *styles_path = "/usr/local/etc/todoStyles.json";

static void interactive_mode(TodoList_t *tlist) {
	Key_t 	pressed_key = 0;
	int 	cursor_pos = 1;
	int 	exit_program = 0;
	cbuf_t *render_buf = cbuf_new(1024);

	while(!exit_program) {
		char input_text_buf[INPUT_TEXT_SIZE];
		char input_key_buf[INPUT_KEY_SIZE];
		int prio;

		memset(input_key_buf, 0, INPUT_KEY_SIZE);
		memset(input_text_buf, 0, INPUT_TEXT_SIZE);
		
		todolist_render(tlist, render_buf, cursor_pos);

		term_render(cbuf_get(render_buf), render_buf->size);

		term_get_key_input(input_key_buf, INPUT_KEY_SIZE);
		
		pressed_key = key_from_raw_input(input_key_buf);

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
				term_get_text_input(input_text_buf, INPUT_TEXT_SIZE);
				todolist_add(tlist, input_text_buf, 0);
				break;
			case KEY_D:
				if(cursor_pos)
					todolist_finish(tlist, &cursor_pos);
				if(cursor_pos > todolist_length(tlist)) cursor_pos--;
				break;
			case KEY_P:
				prio = !todolist_get_priority(tlist, &cursor_pos);
				todolist_set_priority(tlist, &cursor_pos, &prio);
				break;	
			case KEY_ENTER:
				break;
			case KEY_ESC:
				exit_program = 1;
				break;
			case KEY_Q: 
				exit_program = 1;
				break;
			default:
				break;
		}
		cbuf_clear(render_buf);
	}

	cbuf_free(render_buf);
}

int main(int argc, char **argv) {
	FILE *fp;
	TodoList_t tl;
	Command_t cmd;
	StrList_t line_list;
	char *grep_output;
	int i;

	todolist_create(&tl);

	if(!todolist_from_file(&tl, todo_path)) {
		fprintf(stderr, "Can't find todo.txt, maybe you don't have one in the current folder?\n");
		exit(EXIT_FAILURE);
	}

	if(!todolist_load_styles(&tl, styles_path)) {
		fprintf(stderr, "Error parsing styles-file (todoStyles.json)\n");
		exit(EXIT_FAILURE);
	}

	// parse input cmd
	cmd = parser_parse_cmd(argc, argv);
	
	switch(cmd.type) {
		case COMMAND_INVALID:
			printf("%s\n", cmd.data);
			break;
		case COMMAND_LIST:
			todolist_print(&tl);
			break;
		case COMMAND_ADD:
			todolist_add(&tl, (char *)cmd.data, 0);
			//printf("(Todo added) %s", (char *)cmd.data);
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
			grep_output = parser_grep_files((char *)cmd.data, GREP_PATTERN);
			strlist_new(&line_list);
			parser_extract_todos(grep_output, &line_list, GREP_PATTERN);

			for(i = 0; i < line_list.num_lines; i++) {
				if(strlen(line_list.lines[i]) > 1) {
					todolist_add(&tl, line_list.lines[i], 0);	
					printf("(Todo added) %s\n", line_list.lines[i]);
				}
			}

			free(grep_output);
			strlist_free(&line_list);
			break;
		case COMMAND_INTERACTIVE:
			interactive_mode(&tl);
			break;
		default:
			break;
	}
	
	if(!todolist_save(&tl, todo_path)) {
		fprintf(stderr, "sry cant open file\n");
		exit(EXIT_FAILURE);
	}

	todolist_destroy(&tl);
	cmd_free(&cmd);
}

