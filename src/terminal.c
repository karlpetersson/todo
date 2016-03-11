#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include "terminal.h"

static struct termios   save_termios;
static int              term_saved;

static int tty_raw(int fd) {       /* RAW! mode */
    struct termios  buf;

    if (tcgetattr(fd, &save_termios) < 0) /* get the original state */
        return -1;

    buf = save_termios;

    buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
                    /* echo off, canonical mode off, extended input
                       processing off, signal chars off */

    buf.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
                    /* no SIGINT on BREAK, CR-toNL off, input parity
                       check off, don't strip the 8th bit on input,
                       ouput flow control off */

    buf.c_cflag &= ~(CSIZE | PARENB);
                    /* clear size bits, parity checking off */

    buf.c_cflag |= CS8;
                    /* set 8 bits/char */

    buf.c_oflag &= ~(OPOST);
                    /* output processing off */

    buf.c_cc[VMIN] = 1;  /* 1 byte at a time */
    buf.c_cc[VTIME] = 0; /* no timer on input */

    if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
        return -1;

    term_saved = 1;

    return 0;
}


static int tty_reset(int fd) { /* set it to normal! */
    if (term_saved)
        if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
            return -1;

    return 0;
}

static void clrscr() {
	system("clear");
}

void term_init(TerminalState_t *tstate) {
	memset(tstate->writebuf, 0, TERM_WRITE_MAX_SIZE * sizeof(char));
	memset(tstate->readbuf, 0, TERM_READ_MAX_SIZE * sizeof(char));
}	

int term_raw_mode() {
	return tty_raw(STDIN_FILENO);
}

int term_norm_mode() {
	return tty_reset(STDIN_FILENO);
}

void term_wait_for_key(TerminalState_t *tstate) {
	read(STDIN_FILENO, tstate->readbuf, TERM_READ_MAX_SIZE);
}

void term_wait_for_text(TerminalState_t *tstate) {
	term_norm_mode();
	printf("New todo: ");
	fgets(tstate->readbuf, TERM_READ_MAX_SIZE, stdin);
	term_raw_mode();
}

void term_render(TerminalState_t *tstate) {
	tty_reset(STDIN_FILENO);

	clrscr();

	write(STDOUT_FILENO, tstate->writebuf, TERM_WRITE_MAX_SIZE);

	tty_raw(STDIN_FILENO);
}