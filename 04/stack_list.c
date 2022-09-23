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
} stack;

/**
 * @brief cellの初期化.
 */
cell* init_cell() {
  cell* c = (cell*)malloc(sizeof(cell));
  return c;
}

/**
 * @brief stackの初期化.
 */
stack init_stack() {
  stack s;
  s.head = NULL;
  return s;
}

/**
 * @brief 引数のlistをメモリ解放する.
 */
void release_list(stack* s) {
  cell* current = s->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
  s->head = NULL;
}

/**
 * @brief スタックの先頭にvalをプッシュし,stackを更新する.
 */
void push(stack* s, int val) {
  cell* c = init_cell();
  c->element = val;
  c->next = s->head;
  s->head = c;
}

/**
 * @brief スタックの先頭をポップし,stackを更新する.
 */
void pop(stack* s, int* val) {
  if (s == NULL) {
    printf("No more element can be popped from the stack.\n");
    return;
  }
  *val = s->head->element;
  cell* c = s->head;
  s->head = s->head->next;
  free(c);
}

/**
 * @brief 確認用プリント関数.
 */
void print_stack(stack s) {
  printf("LIST CELLS: [ ");
  cell* c = s.head;
  while (c != NULL) {
    printf("%d ", c->element);
    c = c->next;
  }
  printf("]\n");
}

int main() {
  stack s = init_stack();

  for (int i = 0; i < 10; i++) {
    push(&s, i);
  }
  print_stack(s);

  int val;
  pop(&s, &val);
  print_stack(s);
  printf("%d\n", val);

  release_list(&s);
  return 0;
}

// 実行結果
// LIST CELLS: [ 9 8 7 6 5 4 3 2 1 0 ]
// LIST CELLS: [ 8 7 6 5 4 3 2 1 0 ]
// 9
