#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "todo.h"

static const int MAX_LINE_LENGTH = 256;

void todo_print(TodoList_t *tlist) {
	for (int i = 0; i < tlist->num_todos; i++) {
		printf("(%d) %s", (i + 1), tlist->todos[i].text);
	}
}

void todo_parse_file(TodoList_t *tlist) {
	char line[MAX_LINE_LENGTH];
	tlist->num_todos = 0;

	while (fgets(line, MAX_LINE_LENGTH, tlist->file) != NULL) {
		Todo_t todo = todo_create(line);
		tlist->todos[tlist->num_todos] = todo;
       	tlist->num_todos += 1;
	}
}

Todo_t todo_create(char *text) {
	Todo_t todo;
	todo.text = malloc(MAX_LINE_LENGTH * sizeof (char));
	memset(todo.text, 0, MAX_LINE_LENGTH);
	strcat(todo.text, text);
	if(!strchr(todo.text, '\n')) {
		strcat(todo.text, "\n");
	}

	return todo; 
}

void todo_add(TodoList_t *tlist, char *text) {
	Todo_t new_todo = todo_create(text);
	tlist->todos[tlist->num_todos++] = new_todo;
}

void todo_save(TodoList_t *tlist) {
	for (int i = 0; i < tlist->num_todos; i++) {
		fputs(tlist->todos[i].text, tlist->file);
	}
}

void todo_destroy(TodoList_t *tlist) {
	for (int i = 0; i < tlist->num_todos; i++) {
		free(tlist->todos[i].text);
	}
}