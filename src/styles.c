#include "styles.h"

#define ANSI_COLOR_RED 			"\x1b[31m"
#define ANSI_COLOR_BG_GRAY 		"\x1b[40m"
#define ANSI_COLOR_BG_MAGENTA 	"\x1b[45m"
#define ANSI_COLOR_MAGENTA  	"\x1b[35m"
#define ANSI_COLOR_NORMAL 		"\x1b[39m"
#define ANSI_COLOR_BRIGHT 		"\x1b[37m"
#define ANSI_COLOR_RESET 		"\x1b[0m"
#define ANSI_BOLD 				"\x1b[1m"
#define TODO_PRIO_CHAR 			'^'
#define TODO_PRIO_STRING 		"^ "
#define TODO_ID_STRING 			ANSI_COLOR_BRIGHT "%d  " ANSI_COLOR_NORMAL

//TODO: add parameters for prio char, how line number should be rendered etc
void styles_magenta(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected) {
	concat_add(conc, TODO_ID_STRING, linenum);

	if(selected == linenum) {
		concat_add(conc, ANSI_COLOR_BRIGHT);
		concat_add(conc, ANSI_COLOR_BG_MAGENTA);		
	}

	if(prio) {
		if(selected == linenum) {
			concat_add(conc, ANSI_COLOR_BRIGHT TODO_PRIO_STRING ANSI_COLOR_BRIGHT);
		} else {
			concat_add(conc, ANSI_COLOR_MAGENTA TODO_PRIO_STRING ANSI_COLOR_NORMAL);
		}
	}

	concat_add(conc, "%s" ANSI_COLOR_RESET, text);
}

void styles_alt(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected) {
	concat_add(conc, TODO_ID_STRING, linenum);

	if(selected == linenum) {
		concat_add(conc, ANSI_COLOR_BRIGHT);
		concat_add(conc, ANSI_COLOR_BG_GRAY);		
	}

	if(prio) {
		if(selected == linenum) {
			concat_add(conc, ANSI_COLOR_RED TODO_PRIO_STRING ANSI_COLOR_BRIGHT);
		} else {
			concat_add(conc, ANSI_COLOR_RED TODO_PRIO_STRING ANSI_COLOR_NORMAL);
		}
	}

	concat_add(conc, "%s" ANSI_COLOR_RESET, text);
}

void styles_default(MutableConcat_t *conc, const char* text, int linenum, int prio, int selected) {
	if(selected == linenum) {
		concat_add(conc, ANSI_COLOR_BG_GRAY);
	}

	concat_add(conc, TODO_ID_STRING, linenum);

	if(prio) {
		concat_add(conc, ANSI_COLOR_RED TODO_PRIO_STRING ANSI_COLOR_NORMAL);
	}

	concat_add(conc, "%s" ANSI_COLOR_RESET, text);
}
