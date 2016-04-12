#ifndef STYLES_H
#define STYLES_H

#include "concat.h"
#include "cbuf.h"

#define STYLE_LINE_LENGTH (2048)

typedef struct Style_t {
	char line[STYLE_LINE_LENGTH];
	char selected[STYLE_LINE_LENGTH];
	char prioString[65];
} Style_t;

void apply_style(cbuf_t *result, const char *text, int linenum, int prio, int selected, Style_t *style);
int styles_from_json(Style_t *style, const char *path);

#endif