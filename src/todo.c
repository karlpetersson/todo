#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "todo.h"
#include "styles.h"

#define TODO_PRIO_CHAR 			'*'
#define TODO_PRIO_STRING 		"* "

static const int MAX_TODO_LENGTH = 1024;

// list free function
static void todo_free(void *data) {
	free(((Todo_t *)data)->text);
}

// list sort compare function
static int todo_compare(void *v1, void *v2) {
	Todo_t *a = (Todo_t *)v1;
	Todo_t *b = (Todo_t *)v2;
	return (a->prio >= b->prio);
}

// function for counting todos (when used in list_for_each sets p to last index)
static void todo_count(void *data, int index, va_list args) {
	int *p = va_arg(args, int*);
	*p = index;
}

// function for rendering todo to a buffer
static void todo_render(void *data, int index, va_list args) {
	Todo_t				*todo		= (Todo_t *)data;
	//MutableConcat_t		*conc 		= va_arg(args, MutableConcat_t*);
	cbuf_t *			result		= va_arg(args, cbuf_t*);
	int					*selected	= va_arg(args, int*);
	Style_t*			style		= va_arg(args, Style_t*);

	if(style != NULL) {
		apply_style(result, todo->text, index, todo->prio, *selected, style);
	} else {
		printf("No render function specified!");
	}
}

static void todo_save(void *data, int index, va_list args) {
	Todo_t *todo = (Todo_t *)data;
	char buf[MAX_TODO_LENGTH];

	buf[0] = '\0';
	if(todo->prio) {
		snprintf(buf, sizeof buf, TODO_PRIO_STRING);
	}
	strncat(buf, todo->text, MAX_TODO_LENGTH);
	fputs(buf, va_arg(args, FILE*));
}

int todolist_length(TodoList_t *tlist) {
	int count = 0;

	list_for_each(tlist->todos, todo_count, &count);
	
	return count;
}

void todolist_create(TodoList_t *tlist) {
	tlist->todos = list_new(sizeof(Todo_t) + (MAX_TODO_LENGTH * sizeof(char)) + sizeof(int), todo_free);
}

int todolist_from_file(TodoList_t *tlist, const char *path) {
	int 	priority = 0;
	char 	line[MAX_TODO_LENGTH];
	char*	text;
	FILE*	fp;	

	if(!(fp = fopen(path, "r"))) {
		return 0; //TODO: need error codes etc
	}

	//TODO: error prone, need to validate file
	while (fgets(line, MAX_TODO_LENGTH, fp) != NULL) {
		text = line;
		priority = 0;
		if(line[0] == TODO_PRIO_CHAR ) { //&& isspace(line[1])) {
			priority = 1;
			text += strlen(TODO_PRIO_STRING);
		}
		todolist_add(tlist, text, priority);
	}

	fclose(fp);

	return 1;
}

//TODO: leaks memory mby
void todolist_add(TodoList_t *tlist, char *text, int prio) {
	Todo_t todo;

	todo.text = malloc(MAX_TODO_LENGTH * sizeof (char));
	todo.prio = prio;

	memset(todo.text, 0, MAX_TODO_LENGTH);
	strncat(todo.text, text, MAX_TODO_LENGTH);
	
	if(!strchr(todo.text, '\n')) {
		strncat(todo.text, "\n", MAX_TODO_LENGTH);
	}

	list_append(tlist->todos, &todo);
}

void todolist_finish(TodoList_t *tlist, int *linenum) {
	int len = todolist_length(tlist);

	//TODO: err message to user? t 
	if(*linenum <= len && *linenum > 0) {
		list_remove(tlist->todos, *linenum);
	}
}

// sets priority of todo and sorts the list based on priority
void todolist_set_priority(TodoList_t *tlist, int *linenum, int *prio) {
	Todo_t update;
	Todo_t *todo = (Todo_t*)list_get(tlist->todos, *linenum);

	todo->prio = *prio;

	list_set(tlist->todos, *linenum, todo);
	list_sort(tlist->todos, todo_compare);
}

int todolist_get_priority(TodoList_t *tlist, int *linenum) {
	Todo_t *todo = (Todo_t*)list_get(tlist->todos, *linenum);
	return todo->prio;
}

void todolist_render(TodoList_t *tlist, cbuf_t *cbuf, int selected) {
	list_for_each(tlist->todos, todo_render, cbuf, &selected, tlist->style);
}

// prints todos to stdout
void todolist_print(TodoList_t *tlist) {
	cbuf_t *cbuf = cbuf_new(MAX_TODO_LENGTH);
	int selected = 0;
	
	list_for_each(tlist->todos, todo_render, cbuf, &selected, tlist->style);

	printf("%s", cbuf_get(cbuf));
	printf("-----\n");

	cbuf_free(cbuf);
}

// prints each todo in todolist to specified file
int todolist_save(TodoList_t *tlist, const char *path) {
	FILE *fp;
	if(!(fp = fopen(path, "w"))) {
		return 0;
	}

	list_for_each(tlist->todos, todo_save, fp);

	return 1;
}

int todolist_load_styles(TodoList_t *tlist, const char *path) {
	//TODO: if this fails, fall back to default style with err message
	return styles_from_json(&tlist->style, path);
}

void todolist_destroy(TodoList_t *tlist) {
	list_free(tlist->todos);
}