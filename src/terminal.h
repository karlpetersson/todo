#ifndef TERMINAL_H
#define TERMINAL_H

void term_render(char *buf, size_t size);
void term_get_key_input(char *result, size_t size);
void term_get_text_input(char *result, size_t size);

#endif