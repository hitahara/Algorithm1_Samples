#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief データと二分木の2つの枝を保持する構造体.
 */
typedef struct node node;

/**
 * @brief nodeのポインタ.
 */
typedef node* tree;

struct node {
  char element; /** char型で表されたデータ. */
  tree left;    /** 左の枝のnodeのポインタ/tree. */
  tree right;   /** 右の枝のnodeのポインタ/tree. */
};

/**
 * @brief nodeの初期化.
 * @return 初期化された,データやポインタが代入されていないnodeのポインタ.
 */
node* init_node() {
  node* c = (node*)malloc(sizeof(node));
  return c;
}

/**
 * @brief 引数のtreeをメモリ解放する.
 * @param[in] tree メモリ解放するtreeのポインタ.
 */
void release_tree(tree* tree) {
  if (*tree != NULL) {
    release_tree(&(*tree)->left);
    release_tree(&(*tree)->right);
    free(*tree);
  }
  (*tree) = NULL;
}

/**
 * @brief 事前決定された木の生成.
 * @return 生成された木の根本tree．
 */
tree create_tree() {
  tree tree = init_node();
  tree->element = '*';

  node* l = init_node();
  node* r = init_node();
  tree->left = l;
  tree->right = r;
  l->element = '+';
  r->element = '-';

  l = init_node();
  r = init_node();
  tree->left->left = l;
  tree->left->right = r;
  l->element = 'a';
  r->element = 'b';

  l = init_node();
  r = init_node();
  tree->right->left = l;
  tree->right->right = r;
  l->element = 'c';
  r->element = '/';

  l = init_node();
  r = init_node();
  tree->right->right->left = l;
  tree->right->right->right = r;
  l->element = 'd';
  r->element = 'e';

  return tree;
}

/**
 * @brief 前順操作.
 * @param[in] p 操作する対象のtree.
 */
void pre_order(tree p) {
  if (p != NULL) {
    // something
    printf("%c ", p->element);
    // something
    pre_order(p->left);
    pre_order(p->right);
  }
}

/**
 * @brief 間順操作.
 * @param[in] p 操作する対象のtree.
 */
void in_order(tree p) {
  if (p != NULL) {
    in_order(p->left);
    // something
    printf("%c ", p->element);
    // something
    in_order(p->right);
  }
}

/**
 * @brief 後順操作.
 * @param[in] p 操作する対象のtree.
 */
void post_order(tree p) {
  if (p != NULL) {
    post_order(p->left);
    post_order(p->right);
    // something
    printf("%c ", p->element);
    // something
  }
}

int main() {
  tree tree = create_tree();

  pre_order(tree);
  printf("\n");

  in_order(tree);
  printf("\n");

  post_order(tree);
  printf("\n");

  release_tree(&tree);
  pre_order(tree);
  printf("\n");

  return 0;
}