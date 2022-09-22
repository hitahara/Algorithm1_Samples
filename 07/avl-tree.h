#ifndef _avl_tree_h
#define _avl_tree_h
#include <stdbool.h>
#include <stdio.h>

typedef struct record record;
typedef enum direction direction;
typedef struct node node;
typedef node *tree;

record *init_record(size_t key, const char *field);
node *init_node(record *rec);
void dispose(node *n);
void release_tree(tree *rt);

void pre_order(node *p, void (*something)(node *));
void in_order(node *p, void (*something)(node *));
void post_order(node *p, void (*something)(node *));

tree single_rotation_for_insert(tree rt, direction inserted_dir);
tree double_rotation_for_insert(tree rt, direction inserted_dir);
void rebalance_for_insert(tree *rt, direction inserted_dir, bool *grown);
void insert_node(tree *rt, node *np, bool *grown);

tree single_rotation_for_delete(tree rt, direction deleted_dir, bool *shrinked);
tree double_rotation_for_delete(tree rt, direction deleted_dir, bool *shrinked);
void rebalance_for_delete(tree *rt, direction deleted_dir, bool *shrinked);
tree extract_max(tree *rt, bool *shrinked);
void delete_node(tree *rt, size_t target, bool *shrinked);

node *search_node(tree rt, size_t target);

void print_record(record *rec);
void print_node(node *n);
void print_search_node(tree rt, size_t target);
void print_tree_recursion(tree p, size_t depth);
void print_tree(tree rt);
record *cli_record();
void cli_insert(tree *rt);

#endif