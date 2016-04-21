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

int cbuf_puts(cbuf_t *cbuf, const char *str, size_t bytes) {
	int result = CBUF_OK;

	while((cbuf->size + bytes) > cbuf->cap) {
		result = cbuf_grow(cbuf);
	}

	if(result != CBUF_OK) 
		return result;

	memcpy(cbuf->data + cbuf->size, str, bytes);
	cbuf->size += bytes;

	return CBUF_OK;
}

int cbuf_append_space(cbuf_t *cbuf) {
	return cbuf_puts(cbuf, " ", 1);
}

int cbuf_grow(cbuf_t *cbuf) {
	cbuf->data = realloc(cbuf->data, cbuf->chnk);

	if(cbuf->data == NULL)
		return CBUF_NOMEM;

	//memset(cbuf->data + cbuf->chnk, 0, cbuf->chnk);
	cbuf->cap += cbuf->chnk;

	return CBUF_OK;
}

char *cbuf_get(cbuf_t *cbuf) {
	return cbuf->data;
}

void cbuf_clear(cbuf_t *cbuf) {
	assert(cbuf != NULL);

	if (cbuf->data)
		free(cbuf->data);

	cbuf->data = NULL;
	cbuf->size = 0;
	cbuf->cap = 0;
}

void cbuf_free(cbuf_t *cbuf) {
	if(cbuf) {
		if(cbuf->data)
			free(cbuf->data);	
		free(cbuf);
	}
}