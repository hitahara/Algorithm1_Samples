#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief データと次のアイテムを保持する構造体.
 */
typedef struct _cell {
  int element;        /** int型で表されたデータ. */
  struct _cell* next; /** 次のcellを指すポインタ. */
} cell;

/**
 * @brief cellのポインタの型.
 */
typedef struct
{
  cell* head;
} list;

/**
 * @brief cellの初期化.
 */
cell* init_cell() {
  cell* c = (cell*)malloc(sizeof(cell));
  return c;
}

/**
 * @brief listの初期化.
 */
list init_list() {
  list l;
  l.head = NULL;
  return l;
}

/**
 * @brief 引数のlistをメモリ解放する.
 * @param[in] list_head メモリ解放するlistのポインタ.
 */
void release_list(list* stack) {
  cell* current = stack->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
  stack->head = NULL;
}

/**
 * @brief スタックの先頭にvalをプッシュし,stackを更新する.
 * @param[in] stack 先頭にプッシュするlistのポインタ.
 * @param[in] val プッシュするint型のデータ.
 */
void push(list* stack, int val) {
  cell* p = init_cell();
  p->element = val;
  p->next = stack->head;
  stack->head = p;
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
  *val = stack->head->element;
  cell* p = stack->head;
  stack->head = stack->head->next;
  free(p);
}

/**
 * @brief 配列のデータがint型と前提にした,確認用プリント関数.
 * @param[in] list プリントするlistのポインタ.
 */
void print_list(list list) {
  printf("LIST CELLS: [ ");
  cell* c = list.head;
  while (c != NULL) {
    printf("%d ", c->element);
    c = c->next;
  }
  printf("]\n");
}

int main() {
  list stack = init_list();

  for (int i = 0; i < 10; i++) {
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

// 実行結果
// LIST CELLS: [ 9 8 7 6 5 4 3 2 1 0 ]
// LIST CELLS: [ 8 7 6 5 4 3 2 1 0 ]
// 9
