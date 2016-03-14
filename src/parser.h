#ifndef PARSER_H
#define PARSER_H

typedef struct {
	char **lines;
	int num_lines;
} StrList_t;

void strlist_new(StrList_t *sl);
void strlist_add(StrList_t *sl, char *line, size_t length);
void strlist_free(StrList_t *sl);
char *parser_grep_files(char *files, const char *search_pattern);
Command_t parser_parse_cmd(int argc, char *argv[]);
void parser_extract_todos(char *str, StrList_t *sl, const char *search_pattern);

#endif