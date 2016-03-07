#include "list.h"

typedef struct Todo_t {
	char *text;
	int prio;
	//struct Todo_t *next;
} Todo_t;

typedef struct {
	FILE *file;
	List_t *todos;
} TodoList_t;

// create todo.txt
void todo_new_list();

//void todo_free(Todo_t *todo);
void print_todo(void *data, int idx);

// parse todo.txt
Todo_t *todo_create(char *text);

void todo_delete(Todo_t *t);

void todolist_create(TodoList_t *tlist);

void todolist_from_file(TodoList_t *tlist);

// add todo
void todolist_add(TodoList_t *tlist, char *text);

// remove a todo
void todolist_finish(TodoList_t *tlist, int *linenum);

// save todolist to file
void todolist_save(TodoList_t *tlist);

// print contents of todo.txt
void todolist_print(TodoList_t *tlist);

// change priority of todo
void todolist_change_priority(TodoList_t *tlist, int line_nums[]);

void todolist_destroy(TodoList_t *tlist);