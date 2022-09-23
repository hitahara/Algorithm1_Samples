#ifndef _binary_tree_h
#define _binary_tree_h
#include <stdbool.h>
#include <stdio.h>

typedef struct node node;
typedef node *tree;
typedef struct record record;
record *init_record(size_t key, const char *field);
node *init_node(record *rec);
void dispose(node *n);
void release_tree(tree *rt);

void pre_order(tree p, void (*something)(node *));
void in_order(tree p, void (*something)(node *));
void post_order(tree p, void (*something)(node *));

void insert_node(tree *rt, node *np);
tree extract_max_node(tree *rt);
void delete_node(tree *rt, size_t target);
node *search_node(tree rt, size_t target);

record *cli_record();
void cli_insert(tree *rt);
void print_record(record *rec);
void print_node(node *n);
void print_search_node(tree rt, size_t target);
void print_tree_recursion(tree p, size_t depth);
void print_tree(tree rt);

#endif
