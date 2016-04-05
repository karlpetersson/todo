#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "todo.h"

#define ANSI_COLOR_RED 		"\x1b[31m"
#define ANSI_COLOR_BG_GRAY 	"\x1b[40m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_NORMAL 	"\x1b[39m"
#define ANSI_COLOR_BRIGHT 	"\x1b[37m"
#define ANSI_COLOR_RESET 	"\x1b[0m"
#define ANSI_BOLD 			"\x1b[1m"
#define TODO_PRIO_CHAR 		'*'
#define TODO_PRIO_STRING 	"* "
#define TODO_ID_STRING 		ANSI_COLOR_BRIGHT "%d  " ANSI_COLOR_NORMAL

static const int MAX_LINE_LENGTH = 256;

static void __render_alt(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected) {
	concat_add(conc, TODO_ID_STRING, linenum);

	if(selected == linenum) {
		concat_add(conc, ANSI_COLOR_BRIGHT);
		concat_add(conc, ANSI_COLOR_BG_GRAY);		
	}

	if(prio) {
		if(selected == linenum) {
			concat_add(conc, ANSI_COLOR_RED TODO_PRIO_STRING ANSI_COLOR_BRIGHT);
		} else {
			concat_add(conc, ANSI_COLOR_RED TODO_PRIO_STRING ANSI_COLOR_NORMAL);
		}
	}

	concat_add(conc, "%s" ANSI_COLOR_RESET, text);
}

static void __render_default(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected) {
	if(selected == linenum) {
		concat_add(conc, ANSI_COLOR_BG_GRAY);
	}

	concat_add(conc, TODO_ID_STRING, linenum);

	if(prio) {
		concat_add(conc, ANSI_COLOR_RED TODO_PRIO_STRING ANSI_COLOR_NORMAL);
	}

	concat_add(conc, "%s" ANSI_COLOR_RESET, text);
}

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

// function for counting todos (when used in list_foreach sets p to last index)
static void todo_count(void *data, int index, va_list args) {
	int *p = va_arg(args, int*);
	*p = index;
}

// function for printing a todo to stdout
static void todo_print(void *data, int index, va_list args) {
	Todo_t *todo = (Todo_t *)data;
	char buf[MAX_LINE_LENGTH];
	MutableConcat_t conc;

	concat_bind(&conc, buf);
	__render_default(&conc, todo->text, index, todo->prio, 0);
	printf("%s", buf);
}

// function for rendering todo to a buffer
static void todo_render(void *data, int index, va_list args) {
	Todo_t 				*todo 		= (Todo_t *)data;
	MutableConcat_t 	*conc 		= va_arg(args, MutableConcat_t*);
	int 				*selected 	= va_arg(args, int*);
	todo_render_fn_t	renderfn 	= va_arg(args, todo_render_fn_t);

	if(renderfn != NULL) {
		renderfn(conc, todo->text, index, todo->prio, *selected);
	} else {
		printf("No render function specified!");
		//TODO: error handling
	}
}

static void todo_save(void *data, int index, va_list args) {
	Todo_t *todo = (Todo_t *)data;
	char buf[MAX_LINE_LENGTH];

	buf[0] = '\0';
	if(todo->prio) {
		snprintf(buf, sizeof buf, TODO_PRIO_STRING);
	}
	strncat(buf, todo->text, MAX_LINE_LENGTH);
	fputs(buf, va_arg(args, FILE*));
}

int todolist_length(TodoList_t *tlist) {
	int count = 0;
	list_foreach(tlist->todos, todo_count, &count);
	return count;
}

void todolist_create(TodoList_t *tlist) {
	tlist->todos = list_new(sizeof(Todo_t) + (MAX_LINE_LENGTH * sizeof(char)) + sizeof(int), todo_free);
}

int todolist_from_file(TodoList_t *tlist, const char *path) {
	int 	priority = 0;
	char 	line[MAX_LINE_LENGTH];
	char*	text;
	FILE*	fp;	

	if(!(fp = fopen(path, "r"))) {
		return 0; //TODO: need error codes etc
	}

	//TODO: error prone, need to validate file
	while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
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

	todo.text = malloc(MAX_LINE_LENGTH * sizeof (char));
	todo.prio = prio;

	memset(todo.text, 0, MAX_LINE_LENGTH);
	strncat(todo.text, text, MAX_LINE_LENGTH);
	
	if(!strchr(todo.text, '\n')) {
		strncat(todo.text, "\n", MAX_LINE_LENGTH);
	}

	list_append(tlist->todos, &todo);
}

void todolist_finish(TodoList_t *tlist, int *linenum) {
	list_remove(tlist->todos, *linenum);
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

// prints todos to buffer using specified render function
void todolist_render(TodoList_t *tlist, char *buf, int selected) {
	MutableConcat_t concat;
	concat_bind(&concat, buf);
	list_foreach(tlist->todos, todo_render, &concat, &selected, __render_alt);
}

void todolist_print(TodoList_t *tlist) {
	list_foreach(tlist->todos, todo_print);
	printf("-----\n");
}

// prints each todo in todolist to specified file
int todolist_save(TodoList_t *tlist, const char *path) {
	FILE *fp;
	if(!(fp = fopen(path, "w"))) {
		return 0;
	}

	list_foreach(tlist->todos, todo_save, fp);

	return 1;
}

void todolist_destroy(TodoList_t *tlist) {
	list_free(tlist->todos);
}