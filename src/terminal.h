#ifndef TERMINAL_H
#define TERMINAL_H

#define TERM_WRITE_MAX_SIZE (4096)
#define TERM_READ_MAX_SIZE (1024)

typedef struct {
	char writebuf[TERM_WRITE_MAX_SIZE];
	char readbuf[TERM_READ_MAX_SIZE];
} TerminalState_t;

void term_init(TerminalState_t *tstate);
int term_raw_mode();
int term_norm_mode();
void term_render(TerminalState_t *tstate);
void term_get_key_input(TerminalState_t *tstate);
void term_get_text_input(TerminalState_t *tstate);

#endif