#ifndef _sort_algos_h
#define _sort_algos_h
#include <stdbool.h>
#include <stdio.h>

typedef struct record record;
typedef struct sequence sequence;

record *init_record(size_t key, char *field);
sequence *init_sequence();
void release_record(record **rec);
void release_sequence(sequence **seq);

bool is_record_a_larger_than_b(record *a, record *b);
bool is_a_larger_than_b(record **records, size_t a, size_t b);
void compare_and_swap(record **records, size_t a, size_t b);

void insert_tail(sequence *seq, record *rec);

void binary_search_existence_and_index(sequence *seq, size_t target, bool *found, size_t *index);

void simple_sort(sequence *seq);
void bubble_sort(sequence *seq);
void selection_sort(sequence *seq);
void insertion_sort(sequence *seq);

void print_record(record *rec);
void print_sequence(sequence *seq);
void print_search_existence_and_index(sequence *seq, size_t target);

record *cli_record();
void cli_insert(sequence *seq);
void run_sort_algorithm(sequence *seq, void (*sort_algorithm)(sequence *));

void fisher_yates_shuffle(size_t *array, size_t array_size);
#endif