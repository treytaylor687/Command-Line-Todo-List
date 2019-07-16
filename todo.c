#include <stdio.h>
#include <string.h>
#include "todo.h"
#include "useful_strings.h"

void clearscr(void) {
    #ifdef _WIN32
        system("cls");
    #elif defined(unix) || defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
        system("clear");
    #else
        #error "OS not supported."
    #endif
}

// Display the main menu header
void display_main_menu(void) {
    printf(MAIN_MENU_HEADER);
    printf(QUERY);
    printf(OPTIONS_LIST);
	printf(GRAB_INPUT);
}

// Remove all completed items in the list
// If an item is removed, increase removed_count by 1
void remove_completed(void) {
    printf(REMOVE_ITEM_HEADER);
    int removed_count = 0;
    for (int i = 0; i < TODO_LIST_MAX; i++) {
        if (Todo_list[i].title[0] != '\0'
            && Todo_list[i].is_done == 1) {

            // remove item
            strcpy(Todo_list[i].title, "\0");
            strcpy(Todo_list[i].description, "\0");
            Todo_list[i].is_done = 0;
            Todo_list[i].due_day = 0;
            Todo_list[i].due_month = 0;
            Todo_list[i].due_year = 0;

            removed_count++;
        }
    }
    struct todo_list_item_t Todo_list_copy[TODO_LIST_MAX];
    int index = 0;
    for (int i = 0; i < TODO_LIST_MAX; i++) {
		if (Todo_list[i].title[0] != '\0') {
			Todo_list_copy[index++] = Todo_list[i];
		}
    }
    Todo_list_length -= removed_count;
    memcpy(Todo_list, Todo_list_copy, TODO_LIST_MAX);
    printf("Success! %d items removed!\n", removed_count);
}

void add_to_list(void) {
    printf(ADD_TO_LIST_HEADER);
	if (Todo_list_length >= TODO_LIST_MAX) {
		printf(LIST_MAX);
	} else {
    	printf(ADD_TO_LIST);
   		read_todo_list_item(stdin);
    	printf("\n");
	}
}   

// Display the todo list (Option 1)
void print_todo_list(void) {
    printf(TODO_LIST_HEADER);
    for (int i = 0; i < TODO_LIST_MAX; i++) {
        if (Todo_list[i].title[0] == '\0') {
            continue;
        }
        // Print title of entry
        printf("%s\n\n", Todo_list[i].title); 

        // If the item is not done print due date 
        if (Todo_list[i].is_done == 0) {
            // Pad the month with 0 (01), day with 0 (09) and year with 0 (0715) 
            printf("Due: %02d/%02d/%04d\n", Todo_list[i].due_month,
                Todo_list[i].due_day, Todo_list[i].due_year);
        } else {
            printf(COMPLETED);
        }

        printf("Description: %s\n", Todo_list[i].description); 

        printf(LINE_BREAK); 
    }
}

void mark_completed(void) { 
    printf(MARK_ITEM_COMPLETED_HEADER);
    printf(MARK_ITEM_USER_INPUT);
	printf(GRAB_INPUT);
	char buffer[10];
    fgets_no_newline(buffer, 10, stdin);
    int index = atoi(buffer);
	if (index < 0 || index >= TODO_LIST_MAX) {
		exit(1);
	}
    if (Todo_list[index].title[0] != '\0'
		&& Todo_list[index].is_done == 0) {
        Todo_list[index].is_done = 1;
    }
}

void save_to_file(void) {
    printf(SAVE_FILE_HEADER);
    char filename[127];
    printf(INPUT_FILE_NAME);
	printf(GRAB_INPUT);
    fgets_no_newline(filename, 127, stdin);
    FILE *out_file = fopen(filename, "w");
	if (out_file == NULL) {
		exit(1);
	}
    for (int i = 0; i < TODO_LIST_MAX; i++) {
        if (Todo_list[i].title[0] == '\0') {
            continue;
        } else {
			for (int data = 0; data < 6; data++) {
				if (data == 0) {
					fprintf(out_file, "%s\n", Todo_list[i].title);
				} else if (data == 1) {
					fprintf(out_file, "%s\n", Todo_list[i].description);
				} else if (data == 2) {
					fprintf(out_file, "%d\n", Todo_list[i].is_done);
				} else if (data == 3) {
					fprintf(out_file, "%d\n", Todo_list[i].due_day);
				} else if (data == 4) {
					fprintf(out_file, "%d\n", Todo_list[i].due_month);
				} else if (data == 5) {
					fprintf(out_file, "%d\n", Todo_list[i].due_year);
				}
			}
        }
    }
	fclose(out_file);
}

int initialize_program(void) {
	display_main_menu();
    char buffer[10];
    fgets_no_newline(buffer, 10, stdin);
    int command = atoi(buffer);
    if (command == 1) {
        print_todo_list();
        initialize_program();
	} else if (command == 2) {
		add_to_list();
		initialize_program();
	} else if (command == 3) {
		mark_completed();
		initialize_program();
	} else if (command == 4) {
		remove_completed();
		initialize_program();
	} else if (command == 5) {
		save_to_file();
		initialize_program();
	} else if (command == 6) {
		exit(0);
	} else {
		printf(INVALID_CHOICE);
		initialize_program();
	}
	return 0;
}

int main(int argc, const char *argv[]) {
    if (argc > 2) {
        printf(ERROR_USAGE);
        exit(1);
    }
    if (argc == 2) {
        // write file data to Todo_list array
        FILE *in_file = fopen(argv[1], "r");
        if (!in_file) {
            printf("ERROR: File, %s, does not exist!\n", argv[1]);
			exit(1);
        }   
        char buffer[128];
        int i = 0;
        int data = 0;
        while (fgets_no_newline(buffer, 128, in_file) != NULL) {
            switch (data) {
                case 0:
                    strcpy(Todo_list[i].title, buffer);
                    if (Todo_list[i].title[0] != '\0') {
                        Todo_list_length++;
                    }
                    data++;
                    break;
                case 1:
                    strcpy(Todo_list[i].description, buffer);
                    data++;
                    break;
                case 2:
                    Todo_list[i].is_done = atoi(buffer);
                    data++;
                    break;
                case 3:
                    Todo_list[i].due_day = atoi(buffer);
                    data++;
                    break;
                case 4:
                    Todo_list[i].due_month = atoi(buffer);
                    data++;
                    break;
                case 5:
                    Todo_list[i].due_year = atoi(buffer);
                    i++;
                    data = 0;
                    break;
            }
        }
		fclose(in_file);
    }   
    initialize_program();
    return 0;
}



