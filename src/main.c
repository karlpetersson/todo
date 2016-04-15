#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/stat.h>


#include "todo.h"
#include "command.h"
#include "parser.h"
#include "keyhandler.h"
#include "terminal.h"
#include "concat.h"
#include "error.h"

#define GREP_PATTERN "TODO:"
#define INPUT_TEXT_SIZE (1024)
#define INPUT_KEY_SIZE (64)

const char *styles_path = "/usr/local/etc/todoStyles.json";

static int create_txt() {
	//TODO: check for todo.txt file, create if not exists
	/*if(access(todo_path, F_OK) != -1) {
	    return TODO_EALRDYEXISTS;
	} else {
		FILE *fp = fopen(todo_path, "ab+");
		fclose(fp);
	    return TODO_OK;
	}*/
	return TODO_OK;
}

static int find_closest_txt(cbuf_t *res_path) {
	DIR *dp;
	int found_txt = 0;
	struct dirent *entry;
	struct stat statbuf;
 	char cwd[1024];
 	char old_cwd[1024];

	if (!getcwd(old_cwd, sizeof(old_cwd))) {
		printf("GETCWD FAILS");
		return TODO_ENOTXT;
	}
   	
	while(!(strcmp(cwd, "/") == 0) && !found_txt) {
		if((dp = opendir(".")) == NULL) {
			fprintf(stderr,"cannot open directory");
			return TODO_ENOTXT;
		}

		if (!getcwd(cwd, sizeof(cwd))) {
			printf("GETCWD FAILS");
			return TODO_ENOTXT;
		}

		while((entry = readdir(dp)) != NULL) {
			if(strcmp("todo.txt", entry->d_name) == 0) {
				found_txt = 1;
			}
		}
		
		chdir("..");
		closedir(dp);
	}

	if(!found_txt)
		return TODO_ENOTXT;

	cbuf_puts(res_path, cwd, strlen(cwd));
	cbuf_puts(res_path, "/", 1);
	cbuf_puts(res_path, "todo.txt", 10);

	chdir(old_cwd);

	return TODO_OK;
}

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
			case KEY_F:
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
	cbuf_t *txt_path = cbuf_new(1024);
	StrList_t line_list;
	char *grep_output;
	TodoList_t tl;
	todo_error_t res;
	Command_t cmd;
	
	todolist_create(&tl);

	// find closest todo.txt, searching parent directories up to "/"
	find_closest_txt(txt_path);

	// parses todos from todo.txt file
	if((res = todolist_from_file(&tl, cbuf_get(txt_path))) != TODO_OK) {
		print_user_error(res);
		exit(EXIT_FAILURE);
	}

	// loads the currently active style from todoStyles.json
	if((res = todolist_load_styles(&tl, styles_path)) != TODO_OK) {
		print_user_error(res);
		exit(EXIT_FAILURE);
	}

	// parse input cmd
	cmd = parser_parse_cmd(argc, argv);
	
	switch(cmd.type) {
		case COMMAND_INVALID:
			print_user_error(TODO_EINVALIDCMD, cmd.data);
			break;
		case COMMAND_INIT:
			res = create_txt();
			if(res != TODO_OK) {
				print_user_error(res);
				break;
			}
			printf("Created todo.txt\n");
			break;
		case COMMAND_LIST:
			todolist_print(&tl);
			printf("%s\n", cbuf_get(txt_path));
			break;
		case COMMAND_ADD:
			todolist_add(&tl, (char *)cmd.data, 0);
			todolist_print(&tl);
			printf("%s\n", cbuf_get(txt_path));
			break;
		case COMMAND_FINISH:
			res = todolist_finish(&tl, (int *)cmd.data);
			if(res != TODO_OK) {
				print_user_error(res, *((int *)cmd.data)); 
				break;
			}
			todolist_print(&tl);
			printf("%s\n", cbuf_get(txt_path));			
			break;
		case COMMAND_PRIO:
			res = todolist_set_priority(&tl, &((int *)cmd.data)[0], NULL);
			if(res != TODO_OK) {
				print_user_error(res, ((int *)cmd.data)[0]);
				break;
			}
			todolist_print(&tl);
			printf("%s\n", cbuf_get(txt_path));			
			break;
		case COMMAND_LOAD:
			grep_output = parser_grep_files((char *)cmd.data, GREP_PATTERN);
			strlist_new(&line_list);
			parser_extract_todos(grep_output, &line_list, GREP_PATTERN);

			for(int i = 0; i < line_list.num_lines; i++) {
				if(strlen(line_list.lines[i]) > 1) {
					todolist_add(&tl, line_list.lines[i], 0);	
				}
			}

			free(grep_output);
			strlist_free(&line_list);
			todolist_print(&tl);
			printf("%s\n", cbuf_get(txt_path));			
			break;
		case COMMAND_INTERACTIVE:
			interactive_mode(&tl);
			break;
		default:
			break;
	}
	
	if((res = todolist_save(&tl, cbuf_get(txt_path))) != TODO_OK) {
		print_user_error(res);
		exit(EXIT_FAILURE);
	}

	todolist_destroy(&tl);
	cmd_free(&cmd);
}

