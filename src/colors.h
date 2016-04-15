#ifndef COLORS_H_
#define COLORS_H_

static const char *co_table[8] = {
	/*0 = BLACK*/  	"\x1B[30",
	/*1 = RED*/  	"\x1B[31m",
	/*2 = GREEN*/  	"\x1B[32m",
	/*3 = YELLOW*/  "\x1B[33m",
	/*4 = BLUE*/  	"\x1B[34m",
	/*5 = MAGENTA*/ "\x1B[35m",
	/*6 = CYAN*/  	"\x1B[36m",
	/*7 = WHITE*/  	"\x1B[37m"
};

static const char *bg_table[8] = {
	/*0 = GRAY*/ 	"\x1B[40m",
	/*1 = RED*/  	"\x1B[41m",
	/*2 = GREEN*/  	"\x1B[42m",
	/*3 = YELLOW*/  "\x1B[44m",
	/*4 = BLUE*/  	"\x1B[44m",
	/*5 = MAGENTA*/ "\x1B[45m",
	/*6 = CYAN*/  	"\x1B[46m",
	/*7 = WHITE*/  	"\x1B[47m"
};

static const char *st_table[3] = {
	/*0 = BOLD*/ 		"\x1b[1m",
	/*1 = UNDERLINE*/ 	"\x1b[4m",
	/*2 = IMAGE*/ 		"\x1b[7m"
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