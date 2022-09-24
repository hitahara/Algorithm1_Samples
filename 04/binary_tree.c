#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node {
  char element;
  struct _node* left;
  struct _node* right;
} node;

// NOTE:
// 03/linear_list.c と同様に、node と tree を別の構造体
// としており、講義スライドと異なるため注意してください。
// pointer to pointer と dereference を省くことで可読性を
// 上げていますが、再帰関数に対してはデメリットがあり、
// tree 自体を渡すことができません。よって関数には
// tree.root を渡しています。
typedef struct {
  node* root;
} tree;

node* init_node(char element) {
  node* n = (node*)malloc(sizeof(node));
  n->element = element;
  n->left = NULL;
  n->right = NULL;
  return n;
}

void clear_node(node* n) {
  if (n != NULL) {
    clear_node(n->left);
    clear_node(n->right);
    free(n);
  }
}

void clear_tree(tree* t) {
  clear_node(t->root);
  t->root = NULL;
}

tree create_tree() {
  tree t;
  t.root = init_node('*');
  t.root->left = init_node('+');
  t.root->right = init_node('-');
  t.root->left->left = init_node('a');
  t.root->left->right = init_node('b');
  t.root->right->left = init_node('c');
  t.root->right->right = init_node('/');
  t.root->right->right->left = init_node('d');
  t.root->right->right->right = init_node('e');
  return t;
}

void pre_order(node* p) {
  if (p != NULL) {
    printf("%c ", p->element);
    pre_order(p->left);
    pre_order(p->right);
  }
}

void in_order(node* p) {
  if (p != NULL) {
    in_order(p->left);
    printf("%c ", p->element);
    in_order(p->right);
  }
}

void post_order(node* p) {
  if (p != NULL) {
    post_order(p->left);
    post_order(p->right);
    printf("%c ", p->element);
  }
}

int main() {
  tree t = create_tree();

  printf("TREE: [ ");
  pre_order(t.root);
  printf("]\n");

  printf("TREE: [ ");
  in_order(t.root);
  printf("]\n");

  printf("TREE: [ ");
  post_order(t.root);
  printf("]\n");

  clear_tree(&t);

  printf("TREE: [ ");
  pre_order(t.root);
  printf("]\n");

  return 0;
}

// 実行結果
// * + a b - c / d e
// a + b * c - d / e
// a b + c d e / - *
