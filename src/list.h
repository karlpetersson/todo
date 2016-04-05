#ifndef LIST_H
#define LIST_H

#include <stdarg.h>

#define list_for_each_idx(pos, list, idx) \
		for(pos = list->head->next; pos != NULL; pos = pos->next, idx++)

typedef void (*list_cb_t)(void *data, int index, va_list args);
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

List_t *list_new(size_t element_size, list_free_fn_t free_fn);
void list_append(List_t *list, void *element);
void list_prepend(List_t *list, void *element);
void list_remove(List_t *list, int idx);
void *list_get(List_t *list, int idx);
void list_foreach(List_t *list, list_cb_t callback, ...);
void list_set(List_t *list, int idx, void *data);
void list_sort(List_t *list, compare_fn_t fn);
void list_free(List_t *list);

#endif