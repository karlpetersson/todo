#ifndef CONCAT_H
#define CONCAT_H

#include <stdlib.h>

typedef struct {
	char *cur;
	char const *end;
} MutableConcat_t;

typedef struct {
	char *buf;
	char *cur;
	char const *end;
} ImmutableConcat_t;

void concat_bind(MutableConcat_t *c, char *buf);
void concat_add(MutableConcat_t *c, char const* str, ...);

void immConcat_bind(ImmutableConcat_t *c, size_t size);
void immConcat_add(ImmutableConcat_t *c, char const* str, ...);
char *immConcat_get(ImmutableConcat_t *c);
void immConcat_free(ImmutableConcat_t *c);

#endif