#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief データと次のアイテムを保持する構造体.
 */
typedef struct cell {
  int element;       /** int型で表されたデータ. */
  struct cell* next; /** 次のcellを指すポインタ. */
} cell;

/**
 * @brief cellのポインタの型.
 */
typedef cell* list;

/**
 * @brief cellの初期化.
 * @return 初期化された,データやポインタが代入されていないcellのポインタ.
 */
cell* init_cell() {
  cell* c = (cell*)malloc(sizeof(cell));
  return c;
}

/**
 * @brief listの初期化.
 * @return NULLのlist.
 */
list init_list() {
  list l = NULL;
  return l;
}

/**
 * @brief 引数のlistをメモリ解放する.
 * @param[in] list_head メモリ解放するlistのポインタ.
 */
void release_list(list* list_head) {
  cell* disposing_cell = *list_head;
  cell* next_cell = *list_head;
  for (; next_cell != NULL;) {
    next_cell = disposing_cell->next;
    free(disposing_cell);
    disposing_cell = next_cell;
  }
  *list_head = NULL;
}

/**
 * @brief スタックの先頭にvalをプッシュし,stackを更新する.
 * @param[in] stack 先頭にプッシュするlistのポインタ.
 * @param[in] val プッシュするint型のデータ.
 */
void push(list* stack, int val) {
  cell* p = init_cell();
  p->element = val;
  p->next = *stack;
  *stack = p;
}

/**
 * @brief スタックの先頭をポップし,stackを更新する.
 * @param[in] list 先頭をポップするlistのポインタ.
 * @param[in] val ポップした値の受けてとなるint型のポインタ
 */
void pop(list* stack, int* val) {
  if (stack == NULL) {
    printf("No more element can be popped from the stack.\n");
    return;
  }
  *val = (*stack)->element;
  cell* p = *stack;
  *stack = (*stack)->next;
  free(p);
}

/**
 * @brief 配列のデータがint型と前提にした,確認用プリント関数.
 * @param[in] list プリントするlistのポインタ.
 */
void print_list(list list) {
  cell* c = list;
  printf("LIST CELLS: [ ");
  for (; c != NULL; c = c->next) {
    printf("%d ", c->element);
  }
  printf("]\n");
}

int main() {
  list stack = init_list();

  for (int i = 0; i < 20; i++) {
    push(&stack, i);
  }

  print_list(stack);

  int val;
  pop(&stack, &val);

  print_list(stack);
  printf("%d\n", val);

  release_list(&stack);
  return 0;
}