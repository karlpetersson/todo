typedef struct {
	FILE *file;
	Todo_t *todolist;
} TodoList_t;

typedef struct {
	char *text;
	int line_num;
	priority_t prio;
} Todo_t;

// create todo.txt
void todo_new_list();

// parse todo.txt
void todo_parse_file(TodoList_t *tlist);

// add todo to file
void todo_add(TodoList_t *tlist, Todo_t t);

// remove a todo from file
void todo_finish(TodoList_t *tlist, int line_nums[]);

// close file
void todo_save(TodoList_t *tlist);

// print contents of todo.txt
void todo_print(TodoList_t *tlist);

// change priority of todo
void todo_change_priority(TodoList_t *tlist, int line_nums[]);