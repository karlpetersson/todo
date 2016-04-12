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

void term_get_key_input(char *result, size_t size) {
    if(0 != tty_raw(STDIN_FILENO)) {
        printf("raw term fail");
        exit(EXIT_FAILURE);
    }

	read(STDIN_FILENO, result, size);
    
    if(0 != tty_reset(STDIN_FILENO)) {
        printf("reset term fail");
        exit(EXIT_FAILURE);
    }
}

//TODO: add support for load in interactive mode, should reload all src-todos?
void term_get_text_input(char *buf, size_t size) {
	printf("New todo: ");
	fgets(buf, size, stdin);
}

void term_render(char *buf, size_t size) {
	clrscr();    
	write(STDOUT_FILENO, buf, size);
}