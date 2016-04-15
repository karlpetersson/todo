#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "styles.h"
#include "cJSON.h"
#include "error.h"
#include "cbuf.h"
#include "colors.h"

#define ST_RESET "\x1b[0m"

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

static int __parse_style(char *style_raw, cbuf_t *result) {
	char *ptr = NULL;
	char cToStr[2];
	const char *mod;
	int found[3] = {0,0,0};
	int fcnt = 0;

	cToStr[1] = '\0';

	ptr = style_raw;

	while(*ptr) {
		if(*ptr == '$' && *(ptr + 1) == '(') {
			ptr += 2; //skip '$' and '('

			if(*ptr == 'c' || *ptr == 'C') {
				if(!(mod = co_from_id(*(++ptr) - '0'))) return TODO_ESTYLECRNUM;
				cbuf_puts(result, mod, strlen(mod));

			} else if(*ptr == 'b' || *ptr == 'B') {
				if(!(mod = bg_from_id(*(++ptr) - '0'))) return TODO_ESTYLEBGNUM;
				cbuf_puts(result, mod, strlen(mod));

			} else if(*ptr == 's' || *ptr == 'S') {
				if(!(mod = st_from_id(*(++ptr) - '0'))) return TODO_ESTYLESPNUM;
				cbuf_puts(result, mod, strlen(mod));

			} else if(*ptr == 'r' || *ptr == 'R') {
				cbuf_puts(result, ST_RESET, strlen(ST_RESET));

			} else if(*ptr == 'n' || *ptr == 'N') {
				if(fcnt > 2) return TODO_ESTYLEWFRMT;
				found[fcnt++] = 'N';
				cbuf_puts(result, "%d", 2);

			} else if(*ptr == 'p' || *ptr == 'P') {
				if(fcnt > 2) return TODO_ESTYLEWFRMT;
				found[fcnt++] = 'P';
				cbuf_puts(result, "%s", 2);

			} else if(*ptr == 't' || *ptr == 'T') {
				if(fcnt > 2) return TODO_ESTYLEWFRMT;
				found[fcnt++] = 'T';
				cbuf_puts(result, "%s", 2);

			} else {
				printf("Unknown variable: %c \n", *ptr);
			}

			ptr += 2; // skip var and ')'
		} else {
			cToStr[0] = *ptr++;
			cbuf_puts(result, cToStr, strlen(cToStr));
		}
	}

	// reset styles at the end
	cbuf_puts(result, ST_RESET, strlen(ST_RESET));

	// check for required variables
	if(fcnt < 3)
		return TODO_ESTYLEVMSNG;

	// and that they are in correct order
	if(!(found[0] == 'N' && found[1] == 'P' && found[2] == 'T'))
		return TODO_ESTYLEWORDR;

	return TODO_OK;
}

Style_t *styles_init() {
	Style_t *style = malloc(sizeof(Style_t));
	style->text_norm = cbuf_new(STYLE_LINE_LENGTH);
	style->text_sel = cbuf_new(STYLE_LINE_LENGTH);
	style->prio_str = cbuf_new(STYLE_LINE_LENGTH);
	return style;
}

void styles_free(Style_t *style) {
	cbuf_free(style->text_norm);
	cbuf_free(style->text_sel);
	free(style);
}

void styles_apply(cbuf_t *result, const char *text, int linenum, int prio, int selected, Style_t *style) {
	char line[STYLE_LINE_LENGTH];

	sprintf(line, ((selected == linenum) ? cbuf_get(style->text_sel) : cbuf_get(style->text_norm)),
		linenum, (prio ? cbuf_get(style->prio_str) : ""), text);
	
	cbuf_puts(result, line, strlen(line));
}

styles_error_t *styles_from_json(Style_t *style, const char *path) {
	const char *active_style_id;
	char *json_str = __json_str_from_file(path);
	char *prioString;
	cJSON *root, *styles, *active_style;
	todo_error_t res;
	styles_error_t *err_data = NULL;

	if(!json_str) {
		return TODO_ESTYLENOFILE;
	}

	root = cJSON_Parse(json_str);
	active_style_id = cJSON_GetObjectItem(root, "activeStyle")->valuestring;
	styles = cJSON_GetObjectItem(root, "styles");
	
	if((active_style = cJSON_GetObjectItem(styles, active_style_id)) == NULL) {
		print_user_error(TODO_ESTYLENOACTV, active_style_id);
		return TODO_ESTYLE;
	} 

	prioString = cJSON_GetObjectItem(active_style, "prioString")->valuestring;
	cbuf_puts(style->prio_str, prioString, strlen(prioString));

	if((res = __parse_style(cJSON_GetObjectItem(active_style, "line")->valuestring, 
			style->text_norm)) != TODO_OK) {
		print_user_error(res, active_style_id);
		return TODO_ESTYLE;
	}
	if((res = __parse_style(cJSON_GetObjectItem(active_style, "selected")->valuestring, 
			style->text_sel)) != TODO_OK) {
		print_user_error(res, active_style_id);
		return TODO_ESTYLE;
	}

	cJSON_Delete(root);

	return TODO_OK;
}