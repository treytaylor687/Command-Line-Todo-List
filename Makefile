todo: todo.c todo_data.c todo_helpers.c
	gcc -std=c99 -Wall -pedantic -Wextra -Werror -O2 -Wstrict-prototypes -Wold-style-definition -g -o todo todo.c todo_data.c todo_helpers.c

