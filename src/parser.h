#ifndef PARSER_H
#define PARSER_H

// search files for todos
// fork a process that uses grep
void parser_grep_files(char **files, char **results, char *pattern);
Command_t parser_command(int argc, char *argv[]);

#endif