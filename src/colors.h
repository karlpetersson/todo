#ifndef COLORS_H_
#define COLORS_H_

typedef enum {
	BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE,
	BOLD = 0, UNDERLINE = 1, IMAGE = 2
} color_t;

static const char *co_table[8] = {
	[BLACK] 	= "\x1B[30",
	[RED]		= "\x1B[31m",
	[GREEN]		= "\x1B[32m",
	[YELLOW]	= "\x1B[33m",
	[BLUE]		= "\x1B[34m",
	[MAGENTA]	= "\x1B[35m",
	[CYAN]		= "\x1B[36m",
	[WHITE]		= "\x1B[37m"
};

static const char *bg_table[8] = {
	[BLACK] 	= "\x1B[40",
	[RED]		= "\x1B[41m",
	[GREEN]		= "\x1B[42m",
	[YELLOW]	= "\x1B[43m",
	[BLUE]		= "\x1B[44m",
	[MAGENTA]	= "\x1B[45m",
	[CYAN]		= "\x1B[46m",
	[WHITE]		= "\x1B[47m"
};

static const char *st_table[3] = {
	[BOLD]		= "\x1b[1m",
	[UNDERLINE] = "\x1b[4m",
	[IMAGE]		= "\x1b[7m"
};

static inline const char *co_from_id(int id) {
	if (id >= 8 || id < 0) return NULL;
	return co_table[id];
}

static inline const char *bg_from_id(int id) {
	if (id >= 8 || id < 0) return NULL;
	return bg_table[id];
}

static inline const char *st_from_id(int id) {
	if (id >= 3 || id < 0) return NULL;
	return st_table[id];
}

#endif