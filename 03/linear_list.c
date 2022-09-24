#include <stdio.h>
#include <stdlib.h>

/**
 * @brief データと次のデータへのポインタを保持する構造体.
 */
typedef struct _cell {
  int element;
  struct _cell* next;
} cell;

typedef struct
{
  cell* head;
} list;

/**
 * @brief cellの初期化.
 */
cell* init_cell() {
  cell* c = (cell*)malloc(sizeof(cell));
  c->element = 0;
  c->next = NULL;
  return c;
}

/**
 * @brief listのをメモリを解放する.
 */
void release_list(list* l) {
  cell* current = l->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
  l->head = NULL;
}

/**
 * @brief previousの後ろにvalを挿入する. Program 1.3.4
 */
void insert_cell(cell* previous, int val) {
  cell* q = init_cell();
  q->element = val;
  q->next = previous->next;
  previous->next = q;
}

/**
 * @brief リストの先頭にvalを挿入する. Program 1.3.6
 */
void insert_head(list* l, int val) {
  cell* q = init_cell();
  q->element = val;
  q->next = l->head;
  l->head = q;
}

/**
 * @brief previousの後ろのcellを削除する. Program 1.3.7
 */
void delete_cell(cell* previous) {
  cell* q = previous->next;
  previous->next = q->next;
  free(q);
}

/**
 * @brief リストの先頭を削除する. Program 1.3.9
 */
void delete_head(list* l) {
  cell* q = l->head;
  l->head = q->next;
  free(q);
}

/**
 * @brief list確認用プリント関数.
 */
void print_list(list l) {
  printf("LIST: [ ");
  cell* c = l.head;
  while (c != NULL) {
    printf("%d ", c->element);
    c = c->next;
  }
  printf("]\n");
}

int main() {
  list l;
  l.head = NULL;

  cell* example_cell;
  for (int i = 0; i < 10; i++) {
    insert_head(&l, i);
    if (i == 6)
      example_cell = l.head;
  }
  print_list(l);

  insert_cell(example_cell, 100);
  print_list(l);

  delete_cell(example_cell);
  print_list(l);

  delete_head(&l);
  print_list(l);

  release_list(&l);
  print_list(l);

  return 0;
}

// 実行結果
// LIST: [ 9 8 7 6 5 4 3 2 1 0 ]
// LIST: [ 9 8 7 6 100 5 4 3 2 1 0 ]
// LIST: [ 9 8 7 6 5 4 3 2 1 0 ]
// LIST: [ 8 7 6 5 4 3 2 1 0 ]
// LIST: [ ]
