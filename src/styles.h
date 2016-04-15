#ifndef STYLES_H
#define STYLES_H

#include "cbuf.h"

#define STYLE_LINE_LENGTH (2048)

typedef struct Style_t {
	cbuf_t *text_norm;
	cbuf_t *text_sel;
	cbuf_t *prio_str;
} Style_t;


Style_t *styles_init();
void styles_free(Style_t *style);
void styles_apply(cbuf_t *result, const char *text, int linenum, int prio, int selected, Style_t *style);
int styles_from_json(Style_t *style, const char *path);

#endif