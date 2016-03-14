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

static Node_t *iter_here(ListIterator_t *it) {
	return it->cur;
}

void list_sort(List_t *list, compare_fn_t fn) {
	__sort(&list->head->next, fn);
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

void *list_get(List_t *list, int idx) {
	ListIterator_t it;
	void *retval;

	for(iter_bind(&it, list); !iter_done(&it); iter_next(&it)) {
		if(iter_idx(&it) == idx) {
			retval = iter_here(&it)->data;
			break;
		}
	}	

	return retval;
}

void list_set(List_t *list, int idx, void *element) {
	ListIterator_t it;

	for(iter_bind(&it, list); !iter_done(&it); iter_next(&it)) {
		if(iter_idx(&it) == idx) {
			iter_here(&it)->data = element;
			break;
		}
	}
}

void list_foreach(List_t *list, iter_cb_t callback, ...) {
	ListIterator_t it;
	va_list argptr;

	va_start(argptr, callback);
	iter_bind(&it, list);
	for(iter_bind(&it, list); !iter_done(&it); iter_next(&it)) {
		va_list cpy;
		va_copy(cpy, argptr);
		callback(iter_here(&it)->data, iter_idx(&it), cpy);
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

void list_destroy(List_t *list) {
	//TODO: free nodes
}


//void list_free(List_t *list);