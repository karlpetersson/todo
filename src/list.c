#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

static Node_t *iter_here(ListIterator_t *it) {
	return it->cur;
}

void iter_bind(ListIterator_t *it, List_t *list) {
	it->list = list;
	it->cur = list->head->next;
	it->index = 1;
}

void iter_next(ListIterator_t *it) {
	if(it->cur != NULL) {
		it->cur = it->cur->next;
		it->index += 1;
	}
}

int iter_done(ListIterator_t *it) {
	return (it->cur == NULL);
}

int iter_idx(ListIterator_t *it) {
	return it->index;
}

void *iter_value(ListIterator_t *it) {
	return it->cur->data;
}

void list_foreach(List_t *list, iter_cb_t callback) {
	ListIterator_t it;
	iter_bind(&it, list);
	for(iter_bind(&it, list); !iter_done(&it); iter_next(&it)) {
		callback(iter_here(&it)->data, iter_idx(&it));
	}
}

List_t *list_new(size_t element_size, list_free_fn_t free_fn) {
	List_t *list = malloc(sizeof(List_t));
	list->head = malloc(sizeof(Node_t));
	list->head->data = NULL;
	list->head->next = NULL;
	list->free_fn = free_fn;
	list->element_size = element_size;
	return list;
}

void list_prepend(List_t *list, void *element) {
	Node_t *node = malloc(sizeof(Node_t));

	node->data = malloc(list->element_size);
	node->next = NULL;

	memcpy(node->data, element, list->element_size);

	if(list->head->next == NULL) {
		list->head->next = node;
	} else {
		node->next = list->head->next;
		list->head->next = node;
	}
}

void list_append(List_t *list, void *element) {
	Node_t *node = malloc(sizeof (Node_t));
	Node_t *prev = list->head;
	ListIterator_t it;

	node->data = malloc(list->element_size);
	node->next = NULL;

	memcpy(node->data, element, list->element_size);
	
	iter_bind(&it, list);
	while(!iter_done(&it)) {
		prev = iter_here(&it);
		iter_next(&it);
	}

	prev->next = node;
}

void list_remove(List_t *list, int idx) {
	ListIterator_t it;
	Node_t *prev = list->head;

	for(iter_bind(&it, list); !iter_done(&it); iter_next(&it)) {
		if(iter_idx(&it) == idx) {
			prev->next = iter_here(&it)->next;
			list->free_fn(iter_here(&it)->data);
			free(iter_here(&it));
		} else {
			prev = iter_here(&it);
		}
	}
}

//void list_free(List_t *list);