#ifndef CBUF_H
#define CBUF_H

typedef struct _cbuf_t {
	char *data;
	size_t size;
	size_t chnk;
	size_t cap;
} cbuf_t;

cbuf_t* cbuf_new(size_t chunk);
void cbuf_puts(cbuf_t *cbuf, char *str, size_t bytes);
int cbuf_grow(cbuf_t *cbuf);
void cbuf_clear(cbuf_t *cbuf);
void cbuf_free(cbuf_t *cbuf);
char *cbuf_get(cbuf_t *cbuf);

#endif