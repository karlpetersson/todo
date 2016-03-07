#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "todo.h"

static const int MAX_LINE_LENGTH = 256;

static void todo_free(void *data) {
	free(((Todo_t *)data)->text);
}

static void todo_print(void *data, int index) {
	Todo_t *todo = (Todo_t *)data;
	printf("(%d) %s", index, todo->text);
}

void todolist_create(TodoList_t *t) {
	t->todos = list_new(sizeof(Todo_t) + (MAX_LINE_LENGTH * sizeof (char)), todo_free);
}

void todolist_from_file(TodoList_t *tlist) {
	char line[MAX_LINE_LENGTH];
	while (fgets(line, MAX_LINE_LENGTH, tlist->file) != NULL) {
		todolist_add(tlist, line);
	}
}

void todolist_add(TodoList_t *tlist, char *text) {
	Todo_t todo;// = malloc(sizeof (Todo_t));
	todo.text = malloc(MAX_LINE_LENGTH * sizeof (char));
	memset(todo.text, 0, MAX_LINE_LENGTH);
	strcat(todo.text, text);
	if(!strchr(todo.text, '\n')) {
		strcat(todo.text, "\n");
	}
	list_append(tlist->todos, &todo);
}

void todolist_finish(TodoList_t *tlist, int *linenum) {
	list_remove(tlist->todos, *linenum);
}

void todolist_print(TodoList_t *tlist) {
	list_foreach(tlist->todos, todo_print);
}

void todolist_save(TodoList_t *tlist) {
	ListIterator_t it;
	for(iter_bind(&it, tlist->todos); !iter_done(&it); iter_next(&it)) {
		Todo_t *todo = (Todo_t *)iter_value(&it);
		fputs(todo->text, tlist->file);
	}
}

/*
typedef struct {
	TodoList_t *list;
	Todo_t *cur;
	int index;
} Iterator_t;

static const int MAX_LINE_LENGTH = 256;

typedef void (*iter_cb_t)(Todo_t *node);

static void iter_bind(Iterator_t *it, TodoList_t *tlist) {
	it->list = tlist;
	it->cur = tlist->head->next;
	it->index = 1;
}

static void iter_next(Iterator_t *it) {
	if(it->cur != NULL) {
		it->cur = it->cur->next;
		it->index += 1;
	}
}

static int iter_done(Iterator_t *it) {
	return (it->cur == NULL);
}

static Todo_t *iter_here(Iterator_t *it) {
	return it->cur;
}

static int iter_idx(Iterator_t *it) {
	return it->index;
}

static void iter_foreach_reverse(Iterator_t *it, iter_cb_t callback) {
	Todo_t *t;
	if(!iter_done(it)) {
		t = iter_here(it);
		iter_next(it);
		iter_foreach_reverse(it, callback);
		callback(t);
	}
}

static void iter_foreach(Iterator_t *it, iter_cb_t callback) {
	while(!iter_done(it)) {
		callback(iter_here(it));
		iter_next(it);
	}
}

Todo_t *todo_create(char *text) {
	Todo_t *todo = malloc(sizeof (Todo_t));
	todo->text = malloc(MAX_LINE_LENGTH * sizeof (char));
	memset(todo->text, 0, MAX_LINE_LENGTH);
	strcat(todo->text, text);
	if(!strchr(todo->text, '\n')) {
		strcat(todo->text, "\n");
	}

	todo->next = NULL;

	return todo; 
}

void todo_delete(Todo_t *todo) {
	free(todo->text);
	free(todo);
}

void todolist_create(TodoList_t *tlist) {
	tlist->head = malloc(sizeof (Todo_t));
	tlist->head->prio = 2;
	tlist->head->text = NULL;
	tlist->head->next = NULL;
}

void todolist_from_file(TodoList_t *tlist) {
	char line[MAX_LINE_LENGTH];
	while (fgets(line, MAX_LINE_LENGTH, tlist->file) != NULL) {
		todolist_add(tlist, line);
	}
}

void todolist_print(TodoList_t *tlist) {
	Iterator_t it;
	for(iter_bind(&it, tlist); !iter_done(&it); iter_next(&it)) {
		printf("(%d) %s", iter_idx(&it), (iter_here(&it))->text);
	}
}

void todolist_finish(TodoList_t *tlist, int id) {
	Iterator_t it;
	Todo_t *prev = tlist->head;
	for(iter_bind(&it, tlist); !iter_done(&it); iter_next(&it)) {
		if(iter_idx(&it) == id) {
			prev->next = iter_here(&it)->next;
			todo_delete(iter_here(&it));
		}
		prev = iter_here(&it);
	}
}

void todolist_add(TodoList_t *tlist, char *text) {
	Iterator_t it;
	Todo_t *prev = tlist->head;

	iter_bind(&it, tlist);
	while(!iter_done(&it)) {
		prev = iter_here(&it);
		iter_next(&it);
	}

	prev->next = todo_create(text);
}

void todolist_save(TodoList_t *tlist) {
	Iterator_t it;
	for(iter_bind(&it, tlist); !iter_done(&it); iter_next(&it)) {
		fputs(iter_here(&it)->text, tlist->file);
	}
}

void todolist_destroy(TodoList_t *tlist) {
	Iterator_t it;
	iter_bind(&it, tlist);
	iter_foreach_reverse(&it, todo_delete);
}*/

