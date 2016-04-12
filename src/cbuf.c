#include <stdlib.h>
#include <string.h>
#include "cbuf.h"
#include "assert.h"

cbuf_t* cbuf_new(size_t chunk) {
	cbuf_t *cbuf = malloc(sizeof(cbuf_t));
	cbuf->data = NULL;
	cbuf->size = 0;
	cbuf->chnk = chunk;
	cbuf->cap = 0;

	return cbuf;
}

void cbuf_puts(cbuf_t *cbuf, char *str, size_t bytes) {
	int result; 
	
	if((cbuf->size + bytes) > cbuf->cap) {
		result = cbuf_grow(cbuf);
	}

	if(result > 0){}; //OK handle

	memcpy(cbuf->data + cbuf->size, str, bytes);
	cbuf->size += bytes;
}

int cbuf_grow(cbuf_t *cbuf) {
	cbuf->data = realloc(cbuf->data, cbuf->chnk);

	if(cbuf->data == NULL)
		return 0; //ERROR

	cbuf->cap += cbuf->chnk;

	return 1;
}

char *cbuf_get(cbuf_t *cbuf) {
	return cbuf->data;
}

void cbuf_clear(cbuf_t *cbuf) {
	assert(cbuf != NULL);

	free(cbuf->data);

	cbuf->data = NULL;
	cbuf->size = 0;
	cbuf->cap = 0;
}

void cbuf_free(cbuf_t *cbuf) {
	free(cbuf->data);
	free(cbuf);
}