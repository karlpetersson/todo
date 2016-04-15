#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

#include "command.h"
#include "parser.h"
#include "cbuf.h"

//TODO: share errors or specific errors per module??
#define PARSER_INVALID_COMMAND "invalid command %s"
#define PARSER_SYNTAX_ERROR "syntax error, usage: %s"
#define PARSER_ERR_EXC_MAX_LENGTH "input exceeded max length"
#define READ_END 0
#define WRITE_END 1
#define BASE10 10

static const int MAX_LINE_LENGTH = 1024;
static const int MAX_GREP_OUTPUT_LENGTH = 65536;
static const int MAX_ERROR_MSG_LENGTH = 512;

static Command_t parser_error(Command_t *cmd, char const *msg, ...) {
	va_list args;

	cmd_new(cmd, MAX_ERROR_MSG_LENGTH);
	cmd->type = COMMAND_INVALID;

	va_start(args, msg);
	vsnprintf(cmd->data, MAX_ERROR_MSG_LENGTH, msg, args);
	va_end(args);

	return *cmd;
}

void parser_extract_todos(char *str, StrList_t *sl, const char *pattern) {
	const char * 	curLine = str;
	size_t 			pbytes = strlen(pattern);
	cbuf_t 			*result = cbuf_new(MAX_LINE_LENGTH);
	char 			filename[MAX_LINE_LENGTH] = {'\0'};
	char 			todotext[MAX_LINE_LENGTH] = {'\0'};
	char 			*res_ptr, *line_pos, *last_slash_pos;

	while(curLine) {
		const char * nextLine = strchr(curLine, '\n');
		int curLineLen = nextLine ? (nextLine-curLine) : strlen(curLine);
		char * tempStr = (char *) malloc(curLineLen+1);

		if (tempStr) {
			int colon_count = 0;
			int pattern_found = 0;
			int done = 0;

			memcpy(tempStr, curLine, curLineLen);
			tempStr[curLineLen] = '\0';

			line_pos = tempStr;
			last_slash_pos = tempStr;
			res_ptr = filename;

			while(!done && *line_pos) {
				if(*line_pos++ == '/')
					last_slash_pos = line_pos;
				if(*line_pos == ':') 
					done = 1;
			}

			done = 0;
			line_pos = last_slash_pos;

			while(!done && *line_pos) {
				if(*line_pos == ':') {
					colon_count++;
				} 

				*res_ptr++ = *line_pos++;

				if(colon_count == 2) {
					done = 1;
				}
			}

			res_ptr = todotext;

			while(*line_pos) {
				if(pattern_found) {
					*res_ptr++ = *line_pos;
				} else if(strncmp(line_pos, pattern, pbytes) == 0) {
					line_pos += pbytes;
					pattern_found = 1;
				}
				(void)(*line_pos++); // (void) to remove compiler warning
			}

			cbuf_puts(result, filename, strlen(filename));
			cbuf_append_space(result);
			cbuf_puts(result, todotext, strlen(todotext));

			strlist_add(sl, cbuf_get(result), result->size);

			memset(todotext, 0, MAX_LINE_LENGTH);
			memset(filename, 0, MAX_LINE_LENGTH);

			cbuf_clear(result);

			free(tempStr);
		} else {
			printf("malloc() failed!?\n");
		}
		curLine = nextLine ? (nextLine+1) : NULL;
	}

	cbuf_free(result);

}

char *parser_grep_files(char *files, const char *search_pattern) {
	const char *grep_cmd = "grep -n ";
	const char *grep_excl = "--exclude src/main.c";
	cbuf_t 	*grep_input = cbuf_new(MAX_LINE_LENGTH);
	char 	*output = malloc(MAX_GREP_OUTPUT_LENGTH);
	int 	status;
  	int 	fd[2];
	pid_t 	pid;

	cbuf_puts(grep_input, grep_cmd, strlen(grep_cmd)); 
	cbuf_puts(grep_input, search_pattern, strlen(search_pattern));
	cbuf_append_space(grep_input);
	cbuf_puts(grep_input, files, strlen(files));
	cbuf_append_space(grep_input);
	cbuf_puts(grep_input, grep_excl, strlen(grep_excl));

	if(pipe(fd)==-1)
    	fprintf(stderr, "fd FAILED");

  	if((pid = fork()) == -1)
    	fprintf(stderr, "fork FAILED");

    if(pid == 0) {
		dup2(fd[WRITE_END], STDOUT_FILENO);
		close(fd[READ_END]);
		close(fd[WRITE_END]);
   		execl("/bin/sh", "sh", "-c", cbuf_get(grep_input), NULL);
		fprintf(stderr, "execl failed, could not execute /bin/sh -c grep");
		return NULL;
	} else {
		int nbytes = 0;

		close(fd[WRITE_END]);
		nbytes = read(fd[READ_END], output, MAX_GREP_OUTPUT_LENGTH);
		wait(&status);
		cbuf_free(grep_input);

		return output;
	}
}

//find correct command, sanitize input
Command_t parser_parse_cmd(int argc, char **argv) {
	Command_t cmd;

	/* LIST */
	if (argc <= 1) {
		cmd_new(&cmd, 0);
		cmd.type = COMMAND_LIST;
		cmd.data = NULL;
	} 
	/* ADD */
	else if(strcmp(argv[1], "add") == 0 || strcmp(argv[1], "a") == 0) {
		cbuf_t *todo_text = cbuf_new(MAX_LINE_LENGTH);

		if(argc <= 2) {
			cbuf_free(todo_text);
			return parser_error(&cmd, PARSER_SYNTAX_ERROR, "todo a <text>");
		}

		for(int i = 2; i < argc; i++) {
			cbuf_puts(todo_text, argv[i], strlen(argv[i]));
			if(i != (argc - 1 )) cbuf_append_space(todo_text);

			if(todo_text->size > MAX_LINE_LENGTH) {
				cbuf_free(todo_text);
				return parser_error(&cmd, PARSER_ERR_EXC_MAX_LENGTH "(add)");
			}
		}

		cmd_new(&cmd, todo_text->size);	
		strncat(cmd.data, cbuf_get(todo_text), todo_text->size);
		cmd.type = COMMAND_ADD;

		cbuf_free(todo_text);

	/* DO */
	} else if(strcmp(argv[1], "finish") == 0 || strcmp(argv[1], "f") == 0) {
		int p;
		char *end;

		if(argc <= 2) 
			return parser_error(&cmd, PARSER_SYNTAX_ERROR, "todo f <num>");

		p = strtol(argv[2], &end, BASE10);

		if(*end) 
			return parser_error(&cmd, PARSER_SYNTAX_ERROR, "todo f <num>");

		cmd_new(&cmd, sizeof(int));
		cmd.type = COMMAND_FINISH;
		memcpy(cmd.data, &p, sizeof(int));

	/* PRIO */
	} else if(strcmp(argv[1], "prio") == 0 || strcmp(argv[1], "p") == 0) {
		int p[2];
		char *end1, *end2;

		if(argc <= 2)
			return parser_error(&cmd, PARSER_SYNTAX_ERROR, "todo p <num>");
		
		p[0] = strtol(argv[2], &end1, BASE10);
		//p[1] = strtol(argv[3], &end2, BASE10); //prio level, not used atm

		if(*end1) // || *end2)
			return parser_error(&cmd, PARSER_SYNTAX_ERROR, "todo p <num>");
	
		cmd_new(&cmd, sizeof(int));
		cmd.type = COMMAND_PRIO;
		memcpy(cmd.data, &p, sizeof(int));
		
	/* LOAD */
	} else if(strcmp(argv[1], "load") == 0 || strcmp(argv[1], "l") == 0) {
		cbuf_t *filenames = cbuf_new(MAX_LINE_LENGTH);

		if(argc < 3)
			return parser_error(&cmd, PARSER_SYNTAX_ERROR, "todo l <files>");
		
		for(int i = 2; i < argc; i++) {
			cbuf_puts(filenames, argv[i], strlen(argv[i]));
			cbuf_append_space(filenames);
		}

		cmd_new(&cmd, filenames->size);
		cmd.type = COMMAND_LOAD;
		strncat(cmd.data, cbuf_get(filenames), filenames->size);
		cbuf_free(filenames);
		
	} else if(strcmp(argv[1], "i") == 0) {
		cmd_new(&cmd, 0);
		cmd.type = COMMAND_INTERACTIVE;
	} else if(strcmp(argv[1], "n") == 0 || strcmp(argv[1], "new") == 0) {
		cmd_new(&cmd, 0);
		cmd.type = COMMAND_INIT;
	} else {
		parser_error(&cmd, PARSER_INVALID_COMMAND, argv[1]);
	}

	return cmd;
}

void strlist_new(StrList_t *sl) {
	sl->num_lines = 0;
	sl->lines = malloc(sizeof(char *));
}

void strlist_add(StrList_t *sl, char *line, size_t length) {
	sl->num_lines++;
	sl->lines = realloc(sl->lines, sl->num_lines * sizeof(char *));
	sl->lines[sl->num_lines - 1] = malloc(length);
	memset(sl->lines[sl->num_lines - 1], 0, length);
	strncat(sl->lines[sl->num_lines - 1], line, length);
}

void strlist_free(StrList_t *sl) {
	int i;
	for(i = 0; i < sl->num_lines; i++) {
		free(sl->lines[i]);
	}
	free(sl->lines);
}

