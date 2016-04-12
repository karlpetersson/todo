#ifndef TERMINAL_H
#define TERMINAL_H

#define TERM_WRITE_MAX_SIZE (4096)

void term_render(char *buf, size_t size);
void term_get_key_input(char *result, size_t size);
void term_get_text_input(char *result, size_t size);

#endif