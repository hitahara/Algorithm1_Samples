#ifndef _binary_search_array_h
#define _binary_search_array_h
#include <stdbool.h>
#include <stdio.h>

typedef struct record;
typedef struct table table;
record *init_record(size_t key, const char *field);
table *init_table();
void release_table(table *tab);

void linear_search_existence_and_index(table *tab, size_t target, bool *found, size_t *index);
void binary_search_existence_and_index(table *tab, size_t target, bool *found, size_t *index);

void insert_record(table *tab, record *rec);
void delete_record(table *tab, size_t pos);

record *cli_record();
void cli_insert(table *tab);
void print_record(record *rec);
void print_table(table *tab);
void print_search_existence(table *tab, size_t target);

#endif