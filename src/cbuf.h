#ifndef CBUF_H
#define CBUF_H

typedef struct _cbuf_t {
	char *data;
	size_t size;
	size_t chnk;
	size_t cap;
} cbuf_t;

typedef enum {
	CBUF_OK,
	CBUF_NOMEM
} cbuf_error_t;

cbuf_t* cbuf_new(size_t chunk);
int cbuf_puts(cbuf_t *cbuf, const char *str, size_t bytes);
int cbuf_append_space(cbuf_t *cbuf);
int cbuf_grow(cbuf_t *cbuf);
void cbuf_clear(cbuf_t *cbuf);
void cbuf_free(cbuf_t *cbuf);
char *cbuf_get(cbuf_t *cbuf);

#endif