#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief 引数のlistをメモリ解放する.
 * @param[in] list_head メモリ解放するlistのポインタ.
 */
void release_list(list* l) {
  cell* current = l->head;
  cell* next = NULL;
  while (next != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  l->head = NULL;
}

/**
 * @brief previousの後ろにvalを挿入する. Program 1.3.4
 * @param[in] previous 挿入する位置の前のcellをさすポインタ.
 * @param[in] val 挿入するint型のデータ.
 */
void insert_cell(cell* previous, int val) {
  cell* q = init_cell();
  q->element = val;
  q->next = previous->next;
  previous->next = q;
}

/**
 * @brief リストの先頭にvalを挿入し,listを更新する. Program 1.3.6
 * @param[in] list 先頭に挿入するlistのポインタ.
 * @param[in] val 挿入するint型のデータ.
 */
void insert_head(list* l, int val) {
  cell* q = init_cell();
  q->element = val;
  q->next = l->head;
  l->head = q;
}

/**
 * @brief previousの後ろのcellを削除する. Program 1.3.7
 * @param[in] previous 削除するcellの前のcellをさすポインタ.
 */
void delete_cell(cell* previous) {
  cell* q = previous->next;
  previous->next = q->next;
  free(q);
}

/**
 * @brief リストの先頭を削除し,list_headを更新する. Program 1.3.9
 * @param[in] list_head 先頭を削除するlistのポインタ.
 */
void delete_head(list* l) {
  cell* q = l->head;
  l->head = q->next;
  free(q);
}

/**
 * @brief list確認用プリント関数.
 * @param[in] list_head プリントするlist.
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
