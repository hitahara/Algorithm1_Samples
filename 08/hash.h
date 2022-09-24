#ifndef _b_tree_h
#define _b_tree_h
#include <stdbool.h>
#include <stdio.h>

typedef struct record record;
typedef enum status status;
typedef struct hash_record hash_record;
typedef struct hash hash;

record *init_record(int key, const char *field);
hash_record *init_hash_record(record *rec);
hash *init_hash();
void release_hash(hash *h);

int hash_func(int i, int max_size);
void insert(hash *has, record *rec);
void search_existence_and_record(hash *has, int target, bool *found, record **target_rec);

void print_record(record *rec);
void print_hash(hash *h);
void print_search_existence(hash *has, int target);

record *cli_record();
#endif