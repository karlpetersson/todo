#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "command.h"
#include "parser.h"

#define PARSER_ERR "Parser: "
#define PARSER_ERR_NUM_ARGS "Parser: Too few arguments to command "
#define PARSER_ERR_NOT_NUM "Parser: Command argument needs to be a number "
#define READ_END 0
#define WRITE_END 1
#define BASE10 10

static const int MAX_LINE_LENGTH = 1024;
static const int MAX_RESULT_LENGTH = 2048;
static const int MAX_FILENAME_LENGTH = MAX_LINE_LENGTH;
static const int MAX_GREP_OUTPUT_LENGTH = 65536;
static const int MAX_GREP_INPUT_LENGTH = 65536 + (1024 * 4);
static const int MAX_GREP_ARG_LENGTH = 256;
static const int MAX_ERROR_MSG_LENGTH = 512;

static void parser_error(Command_t *cmd, char const *msg) {
	cmd_new(cmd, sizeof(char), MAX_ERROR_MSG_LENGTH);
	cmd->type = COMMAND_INVALID;
	strncat(cmd->data, msg, MAX_ERROR_MSG_LENGTH);
}

void parser_extract_todos(char *str, StrList_t *sl, const char *pattern) {
	const char * 	curLine = str;
	size_t 			pbytes = strlen(pattern);
	char 			filename[MAX_LINE_LENGTH] = {'\0'};
	char 			todotext[MAX_LINE_LENGTH] = {'\0'};
	char 			result[MAX_RESULT_LENGTH] = {'\0'};
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
				*line_pos++;
			}

			memset(result, 0, MAX_RESULT_LENGTH);

			strncat(result, filename, MAX_RESULT_LENGTH);
			strncat(result, " ", MAX_RESULT_LENGTH);
			strncat(result, todotext, MAX_RESULT_LENGTH);

			strlist_add(sl, result, MAX_RESULT_LENGTH);

			memset(todotext, 0, MAX_LINE_LENGTH);
			memset(filename, 0, MAX_LINE_LENGTH);

			free(tempStr);
		} else {
			printf("malloc() failed!?\n");
		}
		curLine = nextLine ? (nextLine+1) : NULL;
	}
}

char *parser_grep_files(char *files, const char *search_pattern) {
	int 	status;
  	int 	fd[2];
	char 	grepstr[MAX_GREP_INPUT_LENGTH] = "grep -n ";
	char 	*output;
	pid_t 	pid;

	output = malloc(MAX_GREP_OUTPUT_LENGTH);
	strncat(grepstr, search_pattern, MAX_GREP_ARG_LENGTH);
	strncat(grepstr, " ", 1);
	strncat(grepstr, files, MAX_GREP_OUTPUT_LENGTH);
	strncat(grepstr, " ", 1);
	strncat(grepstr, "--exclude src/main.c", MAX_GREP_ARG_LENGTH);

	if(pipe(fd)==-1)
    	fprintf(stderr, "fd FAILED");

  	if((pid = fork()) == -1)
    	fprintf(stderr, "fork FAILED");

    if(pid == 0) {
		dup2(fd[WRITE_END], STDOUT_FILENO);
		close(fd[READ_END]);
		close(fd[WRITE_END]);
   		execl("/bin/sh", "sh", "-c", grepstr, NULL);
		fprintf(stderr, "execl failed, could not execute /bin/sh -c grep");
		return NULL;
	} else {
		int nbytes = 0;

		close(fd[WRITE_END]);
		nbytes = read(fd[READ_END], output, MAX_GREP_OUTPUT_LENGTH);
		wait(&status);

		return output;
	}
}

//find correct command, sanitize input
Command_t parser_parse_cmd(int argc, char **argv) {
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
				cmd_new(&cmd, sizeof(char), MAX_LINE_LENGTH);	
				cmd.type = COMMAND_ADD;
		
				//TODO: handle long strings better
				for(int i = 2; i < argc; i++) {
					strncat(cmd.data, argv[i], MAX_LINE_LENGTH);
					if(i != argc) strncat(cmd.data, " ", MAX_LINE_LENGTH);
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
				p[0] = strtol(argv[2], &end1, BASE10);
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
			if(argc < 3) {
				parser_error(&cmd, PARSER_ERR_NUM_ARGS "(load)");
			} else {
				int i;
				cmd_new(&cmd, sizeof(char), MAX_FILENAME_LENGTH * (argc - 2));
				cmd.type = COMMAND_LOAD;
				for(i = 2; i < argc; i++) {
					strncat(cmd.data, argv[i], MAX_FILENAME_LENGTH);
					strncat(cmd.data, " ", 1);
				}
			}
		} else if(strcmp(argv[1], "i") == 0) {
			cmd_new(&cmd, 0, 0);
			cmd.type = COMMAND_INTERACTIVE;
		} else {
			parser_error(&cmd, PARSER_ERR "Unknown command");
		}
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

