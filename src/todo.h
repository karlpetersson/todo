#ifndef TODO_H
#define TODO_H

#include "list.h"
#include "concat.h"
#include "styles.h"
#include "cbuf.h"

//TODO: expand to contain line number etc for source-file todos
typedef struct Todo_t {
	char *text;
	int prio;
} Todo_t;

typedef struct {
	FILE *file;
	List_t *todos;
	Style_t *style;
} TodoList_t;

void todolist_create(TodoList_t *tlist);
void todolist_add(TodoList_t *tlist, char *text, int prio);
int todolist_finish(TodoList_t *tlist, int *linenum);
void todolist_print(TodoList_t *tlist);
int todolist_set_priority(TodoList_t *tlist, int *linenum, int *prio);
void todolist_load(TodoList_t *tlist, char *files);
void todolist_render(TodoList_t *tlist, cbuf_t *buf, int selected);
void todolist_destroy(TodoList_t *tlist);
int todolist_from_file(TodoList_t *tlist, const char *path);
int todolist_save(TodoList_t *tlist, const char *path);
int todolist_length(TodoList_t *tlist);
int todolist_get_priority(TodoList_t *tlist, int *linenum);
int todolist_load_styles(TodoList_t *tlist, const char *path);

#endif