typedef struct {
	char *text;
	int id;
	int prio;
} Todo_t;

typedef struct {
	FILE *file;
	Todo_t todos[1024];
	int num_todos;
} TodoList_t;

// create todo.txt
void todo_new_list();

// parse todo.txt
void todo_parse_file(TodoList_t *tlist);

Todo_t todo_create(char *text);

// add todo
void todo_add(TodoList_t *tlist, char *text);

// remove a todo
void todo_finish(TodoList_t *tlist, int line_nums[]);

// save todolist to file
void todo_save(TodoList_t *tlist);

// print contents of todo.txt
void todo_print(TodoList_t *tlist);

// change priority of todo
void todo_change_priority(TodoList_t *tlist, int line_nums[]);

void todo_destroy(TodoList_t *tlist);