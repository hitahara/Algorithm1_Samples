#ifndef _b_tree_h
#define _b_tree_h
#include <stdbool.h>
#include <stdio.h>

typedef struct record record;
typedef enum node_type node_type;
typedef struct key_branch_pair key_branch_pair;
typedef struct internal_data internal_data;
typedef struct b_node b_node;
typedef b_node *b_tree;

record *init_record(int key, const char *field);
key_branch_pair *init_pair();
internal_data *init_internal_data();
b_node *init_b_node_leaf(record *rec);
b_node *init_b_node_internal();

void shift_b_node_internal(b_node *shifting_node, int min_condition_key);
void b_node_ins(b_node **bn, record *inserting_rec, key_branch_pair **parent_pair, bool *parent_expanded);
void b_node_insert(b_tree *b_rt, record *inserting_rec);

void binary_search_existence_and_index(b_node *n, int target, bool *found, int *index);

void print_record(record *rec);
void print_leaf_node(b_node *n);
void print_internal_node(b_node *n);
void print_tree_recursion(b_tree p, int depth);
void print_tree(b_tree rt);
void print_search_node(b_tree rt, int target);

record *cli_record();
void cli_insert(b_tree *rt);

#endif