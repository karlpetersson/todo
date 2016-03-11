#ifndef LIST_H
#define LIST_H

#include <stdarg.h>

typedef void (*iter_cb_t)(void *data, int index, va_list args);
typedef void (*list_free_fn_t)(void *);
typedef int (*compare_fn_t)(void *v1, void *v2);

typedef struct _Node_t {
	void *data;
	struct _Node_t *next;
} Node_t;

typedef struct {
	Node_t *head;
	size_t element_size;
	list_free_fn_t free_fn;
} List_t;

typedef struct {
	List_t *list;
	Node_t *cur;
	int index;
} ListIterator_t;

List_t *list_new(size_t element_size, list_free_fn_t free_fn);
void list_append(List_t *list, void *element);
void list_prepend(List_t *list, void *element);
void list_remove(List_t *list, int idx);
void list_destroy(List_t *list);
void *list_get(List_t *list, int idx);
void list_foreach(List_t *list, iter_cb_t callback, ...);
void list_set(List_t *list, int idx, void *data);
void list_sort(List_t *list, compare_fn_t fn);

//TODO: expose iterator interface or not?
void iter_bind(ListIterator_t *it, List_t *list);
void iter_next(ListIterator_t *it);
int iter_done(ListIterator_t *it);
int iter_idx(ListIterator_t *it);
void *iter_value(ListIterator_t *it);

#endif