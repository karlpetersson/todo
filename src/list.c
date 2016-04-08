#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

static Node_t *__merge(Node_t *a, Node_t *b, compare_fn_t fn) {
	Node_t *result;

	if (a == NULL)
		return b;
	else if (b == NULL)
		return a;

	if(fn(a->data, b->data)) {
		result = a;
		result->next = __merge(a->next, b, fn);
	} else {
		result = b;
		result->next = __merge(a, b->next, fn);
	}

	return result;
}

static void __split(Node_t *source, Node_t **front_ref, Node_t **back_ref) {
	Node_t *slow;
	Node_t *fast;

	if(source == NULL || source->next == NULL) {
		*front_ref = source;
		*back_ref = NULL;
	} else {
		slow = source;
		fast = source->next;
		while(fast != NULL) {
			fast = fast->next;
			if(fast != NULL) {
				slow = slow->next;
				fast = fast->next;
			}
		}
	}

	*front_ref = source;
	*back_ref = slow->next;
	slow->next = NULL; // "delink" the sublists
}

static void __sort(Node_t **headref, compare_fn_t fn) {
	Node_t *head = *headref;
	Node_t *a;
	Node_t *b;

	// list is 0 or 1 length
	if((head == NULL) || (head->next == NULL))
		return;

	__split(head, &a, &b);

	__sort(&a, fn);
	__sort(&b, fn);

	*headref = __merge(a, b, fn);
}

void list_sort(List_t *list, compare_fn_t fn) {
	__sort(&list->head->next, fn);
}

void *list_get(List_t *list, int get_idx) {
	Node_t *cur;
	int idx = 1;
	void *retval;

	__list_for_each_idx(cur, list, idx) {
		if(idx == get_idx) {
			retval = cur->data;
			break;
		}
	}

	return retval;
}

void list_set(List_t *list, int set_idx, void *element) {
	Node_t *cur;
	int idx = 1;

	__list_for_each_idx(cur, list, idx) {
		if(idx == set_idx) {
			cur->data = element;
			break;
		}
	}
}

void list_for_each(List_t *list, list_cb_t callback, ...) {
	va_list argptr;
	Node_t *cur;
	int idx = 1;

	va_start(argptr, callback);
	__list_for_each_idx(cur, list, idx) {
		va_list cpy;
		va_copy(cpy, argptr);
		callback(cur->data, idx, cpy);
		va_end(cpy);
	}
	va_end(argptr);
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
	Node_t *search = list->head;

	node->data = malloc(list->element_size);
	node->next = NULL;
	memcpy(node->data, element, list->element_size);

	while(search->next != NULL) {
		search = search->next;
	}

	search->next = node;
}

void list_remove(List_t *list, int idx) {
	Node_t *prev = list->head;
	Node_t *rem = list->head;

	while(rem != NULL) {
		if(idx-- == 0) {
			prev->next = rem->next;
			list->free_fn(rem->data);
			free(rem);
		} else {
			prev = rem;
			rem = rem->next;			
		}
	}
}

void list_free(List_t *list) {
	Node_t *cur = list->head->next;

	while(cur != NULL) {
		Node_t *del = cur;
		cur = cur->next;
		list->free_fn(del->data);
		free(del);
	}
}