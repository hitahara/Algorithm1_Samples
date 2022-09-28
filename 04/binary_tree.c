#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NOTE: list では 可読性のために node と list を
// 別の構造体にしていました。しかし、別の構造体に
// すると再帰関数を定義できないというデメリットがあります。
// 今回の木構造では再帰関数が多用されるため別の構造体に
// しません。サンプルでは混乱回避のために tree 自体を定義
// しませんが typedef を使うと講義スライドと同様になります。
typedef struct _node {
  char element;
  struct _node* left;
  struct _node* right;
} node;

node* init_node(char element) {
  node* n = (node*)malloc(sizeof(node));
  n->element = element;
  n->left = NULL;
  n->right = NULL;
  return n;
}

// NOTE: list では node と別の構造体にすることで
// pointer to pinter と dereference を回避していました。
// 木構造では、これらを回避するとかなりコードが肥大化
// してしまうため、これらを使うことにしています。
void clear(node** p_current) {
  node* current = *p_current;
  if (current != NULL) {
    clear(&current->left);
    clear(&current->right);
    free(current);
    *p_current = NULL;
  }
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

node* create_root() {
  node* root = init_node('*');
  root->left = init_node('+');
  root->right = init_node('-');
  root->left->left = init_node('a');
  root->left->right = init_node('b');
  root->right->left = init_node('c');
  root->right->right = init_node('/');
  root->right->right->left = init_node('d');
  root->right->right->right = init_node('e');
  return root;
}

int main() {
  node* root = create_root();

  printf("TREE: [ ");
  pre_order(root);
  printf("]\n");

  printf("TREE: [ ");
  in_order(root);
  printf("]\n");

  printf("TREE: [ ");
  post_order(root);
  printf("]\n");

  clear(&root);

  printf("TREE: [ ");
  pre_order(root);
  printf("]\n");

  return 0;
}

// 実行結果
// * + a b - c / d e
// a + b * c - d / e
// a b + c d e / - *
