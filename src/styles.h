#ifndef STYLES_H
#define STYLES_H

#include "concat.h"

void styles_magenta(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected);
void styles_default(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected);
void styles_alt(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected);

#endif