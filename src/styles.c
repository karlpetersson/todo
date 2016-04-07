#include <stdio.h>
#include <string.h>
#include "styles.h"
#include "cJSON.h"

#define BG_GRAY 	"\x1B[40m"
#define BG_RED  	"\x1B[41m"
#define BG_GREEN  	"\x1B[42m"
#define BG_YELLOW  	"\x1B[44m"
#define BG_BLUE  	"\x1B[44m"
#define BG_MAGENTA  "\x1B[45m"
#define BG_CYAN  	"\x1B[46m"
#define BG_WHITE  	"\x1B[47m"

#define CO_BLACK  	"\x1B[30"
#define CO_RED  	"\x1B[31m"
#define CO_GREEN  	"\x1B[32m"
#define CO_YELLOW  	"\x1B[33m"
#define CO_BLUE  	"\x1B[34m"
#define CO_MAGENTA  "\x1B[35m"
#define CO_CYAN  	"\x1B[36m"
#define CO_WHITE  	"\x1B[37m"

#define ST_NORMAL 		"\x1b[39m"
#define ST_RESET 		"\x1b[0m"
#define ST_BOLD 		"\x1b[1m"
#define ST_FAINT 		"\x1b[2m"
#define ST_ITALIC 		"\x1b[3m"
#define ST_UNDERLINE 	"\x1b[4m"
#define ST_BLINK 		"\x1b[5m"
#define ST_FBLINK 		"\x1b[6m"
#define ST_IMAGE 		"\x1b[7m"
#define ST_CONCEAL 		"\x1b[8m"
#define ST_CROSS 		"\x1b[9m"

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

static char *__json_str_from_file(const char *filename) {
	char *json_str;
	long file_size;
	FILE *fp;
	
	if((fp = fopen(filename, "rb")) == NULL) {
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);

	json_str = malloc(file_size * (sizeof(char)));
	
	fread(json_str, sizeof(char), file_size, fp);
	fclose(fp);

	return json_str;
}

static void __parse_style(char *style_raw, char *result) {
	char *ptr = NULL;
	char cToStr[2];
	int num;

	cToStr[1] = '\0';

	strcat(style_raw, "$(R)");

	ptr = style_raw;

	while(*ptr) {
		if(*ptr == '$' && *(ptr + 1) == '(') {
			ptr += 2; //skip '$' and '('

			if(*ptr == 'c') {
				num = *(++ptr) - '0';
				strcat(result, co_table[num]);
			} else if(*ptr == 'b') {
				num = *(++ptr) - '0';
				strcat(result, bg_table[num]);
			} else if(*ptr == 's') {
				num = *(++ptr) - '0';
				strcat(result, st_table[num]);
			} else if(*ptr == 'R') {
				strcat(result, ST_RESET);
			} else if(*ptr == 'N') {
				strcat(result, "%d");
			} else if(*ptr == 'P') {
				strcat(result, "%s");
			} else if(*ptr == 'T') {
				strcat(result, "%s");
			} else {
				printf("Style parser error: unknown variable: %c \n", *ptr);
			}

			ptr += 2; // skip var and ')'
		} else {
			cToStr[0] = *ptr++;
			strcat(result, cToStr);
		}
	}
}

void apply_style(MutableConcat_t *conc, const char *text, int linenum, int prio, int selected, Style_t *style) {

	char line[STYLE_LINE_LENGTH];

	sprintf(line, ((selected == linenum) ? style->selected : style->line),
		linenum, (prio ? style->prioString : ""), text); 
		
	concat_add(conc, "%s" ST_RESET, line);
}

int styles_from_json(Style_t *style, const char *path) {
	const char *active_style_id;
	char *json_str = __json_str_from_file(path);

	if(!json_str) return 0;

	char style_str[STYLE_LINE_LENGTH] = {0};
	char style_str_sel[STYLE_LINE_LENGTH] = {0};	
	cJSON *root, *styles, *active_style;

	root = cJSON_Parse(json_str);
	active_style_id = cJSON_GetObjectItem(root, "activeStyle")->valuestring;
	styles = cJSON_GetObjectItem(root, "styles");
	active_style = cJSON_GetObjectItem(styles, active_style_id);

	style->line[0] = '\0';
	style->selected[0] = '\0';
	style->prioString[0] = '\0';

	strncat(style_str, cJSON_GetObjectItem(active_style, "line")->valuestring, 1024);
	strncat(style_str_sel, cJSON_GetObjectItem(active_style, "selected")->valuestring, 1024);
	strncat(style->prioString, cJSON_GetObjectItem(active_style, "prioString")->valuestring, 64);

	__parse_style(style_str, style->line);
	__parse_style(style_str_sel, style->selected);

	cJSON_Delete(root);

	return 1;
}