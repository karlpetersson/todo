#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "concat.h"


void concat_bind(MutableConcat_t *c, char *buf) {
	c->cur = buf;
	c->end = buf + sizeof buf;
}

void concat_add(MutableConcat_t *c, size_t size, char const* str, ...) {
	va_list argp;
	va_start(argp, str);
	c->cur += vsnprintf(c->cur, size, str, argp);
	va_end(argp);
}

void immConcat_bind(ImmutableConcat_t *c, size_t size) {
	c->buf = malloc(size * sizeof(char));
	c->cur = c->buf;
	c->end = c->buf + size;
}

void immConcat_add(ImmutableConcat_t *c, char const* str, ...) {
	va_list argp;
	va_start(argp, str);
	c->cur += vsprintf(c->cur, str, argp);
	va_end(argp);
}

char* immConcat_get(ImmutableConcat_t *c) {
	return c->buf;
}

void immConcat_free(ImmutableConcat_t *c) {
	free(c->buf);
}
