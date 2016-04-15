#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include <stdarg.h>

#define MAX_ERR_LEN 128

typedef enum {
	TODO_OK,
	TODO_ENOMEM,
	TODO_ENOID,
	TODO_ENOTXT,
	TODO_EALRDYEXISTS,
	TODO_EINVALIDCMD,
	TODO_ESTYLE,
	TODO_ESTYLENOFILE,
	TODO_ESTYLENOACTV,
	TODO_ESTYLEPARSE,
	TODO_ESTYLECRNUM,
	TODO_ESTYLEBGNUM,
	TODO_ESTYLESPNUM,
	TODO_ESTYLEWFRMT,
	TODO_ESTYLEVMSNG,
	TODO_ESTYLEWORDR,
	TODO_NUMERR
} todo_error_t;

static const char *error_msg_table[MAX_ERR_LEN] = {
	[TODO_ENOMEM]		=	"todo: Malloc failed",
	[TODO_ENOID] 		=	"todo: ID %d not found\n",
	[TODO_ENOTXT] 		=	"todo: Can't find todo.txt\n",
	[TODO_EALRDYEXISTS] =	"todo: Todo.txt already exists in current folder\n",
	[TODO_EINVALIDCMD]	=	"todo: %s\n", //err str supplied by parser
	[TODO_ESTYLE]		=	"todo: Error parsing styles\n",
	[TODO_ESTYLENOFILE] =	"todo: Can't open file todoStyles.json\n",
	[TODO_ESTYLENOACTV] =	"todo: Active style '%s' is not defined\n",
	[TODO_ESTYLEPARSE] 	=	"todo: Error parsing styles\n",
	[TODO_ESTYLECRNUM] 	=	"todo: style '%s': Color value out of bounds (0-7 allowed)\n",
	[TODO_ESTYLEBGNUM] 	=	"todo: style '%s': Background color value out of bounds (0-7 allowed)\n",
	[TODO_ESTYLESPNUM] 	=	"todo: style '%s': Special value out of bounds (0-2 allowed)\n",
	[TODO_ESTYLEWFRMT] 	=	"todo: style '%s': Style contains more than one of each $(N) $(P) and $(T)\n",
	[TODO_ESTYLEVMSNG] 	=	"todo: style '%s': One of the variables $(N) $(P) $(T) is missing\n",
	[TODO_ESTYLEWORDR] 	=	"todo: style '%s': Variables $(N) $(P) $(T) is in wrong order\n"
};

static inline void print_user_error(todo_error_t errnum, ...) {
	va_list args;

	va_start(args, errnum);
	vprintf(error_msg_table[errnum], args);
	va_end(args);
}

#endif