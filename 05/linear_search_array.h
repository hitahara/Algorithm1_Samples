#ifndef _linear_search_array_h
#define _linear_search_array_h
#include <stdbool.h>
#include <stdio.h>

void fisher_yates_shuffle(int* array, int array_size);

typedef struct record record;
typedef struct table table;  // array用のtable,list用と異なる.

record* init_record(int key, const char* field);
table* init_table();
void release_table(table* tab);

void insert_tail(table* tab, record* rec);
void delete_record(table* tab, int pos);

bool search_existence(table* tab, int target);
int search_index(table* tab, int target);

record* cli_record();
void cli_insert_tail(table* tab);
void print_record(record* rec);
void print_table(table* tab);
void print_search_existence(table* tab, int target);

#endif