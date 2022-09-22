#ifndef _linear_search_list_h
#define _linear_search_list_h
#include <stdio.h>
#include <stdbool.h>

void fisher_yates_shuffle(size_t* array, size_t array_size);

typedef struct record record;
typedef struct cell cell;
typedef cell* list;
typedef struct table table;//list用のtable,array用と異なる.

record* init_record(size_t key, const char* field);//int -> size_t
cell* init_cell(record* rec);
list init_list();
table* init_table();
void dispose(cell* c);
void release_table(table* tab);

void insert_head_list(table* tab, list p);
void insert_head_record(table* tab, record* rec);
void delete_next(table* tab, list previous);
void delete_target(table* tab, size_t target);

bool search_existence(table* tab, size_t target);
list search_previous(table* tab, size_t target);

record* cli_record();
void cli_insert_head(table* tab);
void print_record(record* rec);
void print_table(table* tab);
void print_search_existence(table* tab, size_t target);

#endif