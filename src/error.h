#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <stdarg.h>

#define MAX_ERR_LEN 128

typedef enum {
	TODO_OK,
	TODO_ENOID,
	TODO_ENOTXT,
	TODO_EINVALIDCMD,
	TODO_ESTYLE,
	TODO_ESTL_NOFILE,
	TODO_ESTL_NOACTV,
	TODO_ESTL_PARSE,
	TODO_ESTL_CRNUM,
	TODO_ESTL_BGNUM,
	TODO_ESTL_SPNUM,
	TODO_ESTL_WFRMT,
	TODO_ESTL_VMSNG,
	TODO_ESTL_WORDR,
	TODO_NUMERR
} todo_error_t;

static const char *error_msg_table[MAX_ERR_LEN] = {
	[TODO_ENOID] 		=	"todo: ID %d not found\n",
	[TODO_ENOTXT] 		=	"todo: Can't find todo.txt\n",
	[TODO_EINVALIDCMD]	=	"todo: %s\n", //err str supplied by parser
	[TODO_ESTYLE]		=	"todo: error parsing styles\n",
	[TODO_ESTL_NOFILE] 	=	"todo: Can't open file todoStyles.json\n",
	[TODO_ESTL_NOACTV] 	=	"styles: active style '%s' is not defined in the array of styles\n",
	[TODO_ESTL_PARSE] 	=	"todo: error parsing styles\n",
	[TODO_ESTL_CRNUM] 	=	"style '%s': Color value out of bounds (0-7 allowed)\n",
	[TODO_ESTL_BGNUM] 	=	"style '%s': Background color value out of bounds (0-7 allowed)\n",
	[TODO_ESTL_SPNUM] 	=	"style '%s': Special value out of bounds (0-2 allowed)\n",
	[TODO_ESTL_WFRMT] 	=	"style '%s': Style contains more than one of each $(N) $(P) and $(T)\n",
	[TODO_ESTL_VMSNG] 	=	"style '%s': One of the variables $(N) $(P) $(T) is missing\n",
	[TODO_ESTL_WORDR] 	=	"style '%s': Variables for line number, priority and text is in wrong order (correct order is: $(N) $(P) $(T))"
};

static inline void print_user_error(todo_error_t errnum, ...) {
	va_list args;

	va_start(args, errnum);
	vprintf(error_msg_table[errnum], args);
	va_end(args);
}

#endif