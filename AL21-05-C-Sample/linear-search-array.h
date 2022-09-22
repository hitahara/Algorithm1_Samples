#ifndef _linear_search_array_h
#define _linear_search_array_h
#include <stdio.h>
#include <stdbool.h>

void fisher_yates_shuffle(size_t* array, size_t array_size);

typedef struct record record;
typedef struct table table;//array用のtable,list用と異なる.



record* init_record(size_t key, const char* field);
table* init_table();
void release_table(table* tab);




void insert_tail(table* tab, record* rec);
void delete_record(table* tab, size_t pos);



bool search_existence(table* tab, size_t target);
size_t search_index(table* tab, size_t target);

record* cli_record();
void cli_insert_tail(table* tab);
void print_record(record* rec);
void print_table(table* tab);
void print_search_existence(table* tab, size_t target);

#endif