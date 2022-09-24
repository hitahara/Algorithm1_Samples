#include <stdio.h>
#include <stdlib.h>

typedef struct _cell {
  int element;
  struct _cell* next;
} cell;

// NOTE:
// 講義スライドでは list は cell* の別名ですが、このコードでは
// 別の構造体としています。別名としてしまうと head のポインタ
// 自体を変更する操作で pointer to pointer と dereference が
// 必要になり、可読性が損なわれます。それに対して list の内部に
// cell* を持つ設計にすると上記の問題が解決されるうえ、head と
// tail の両方を持つようなデータ構造にも容易に拡張できます。
typedef struct
{
  cell* head;
} list;

void release_list(list* l) {
  cell* current = l->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
  l->head = NULL;
}

void insert_cell(cell* previous, int val) {
  cell* c = (cell*)malloc(sizeof(cell));
  c->element = val;
  c->next = previous->next;
  previous->next = c;
}

void insert_head(list* l, int val) {
  cell* c = (cell*)malloc(sizeof(cell));
  c->element = val;
  c->next = l->head;
  l->head = c;
}

void delete_cell(cell* previous) {
  cell* target = previous->next;
  previous->next = target->next;
  free(target);
}

void delete_head(list* l) {
  cell* target = l->head;
  l->head = target->next;
  free(target);
}

void print_list(list l) {
  printf("LIST: [ ");
  cell* current = l.head;
  while (current != NULL) {
    printf("%d ", current->element);
    current = current->next;
  }
  printf("]\n");
}

int main() {
  list l;
  l.head = NULL;

  cell* c;
  for (int i = 0; i < 10; i++) {
    insert_head(&l, i);
    if (i == 6)
      c = l.head;
  }
  print_list(l);

  insert_cell(c, 100);
  print_list(l);

  delete_cell(c);
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
