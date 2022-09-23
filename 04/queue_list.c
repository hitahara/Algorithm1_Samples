#include <stdbool.h>
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

typedef struct {
  cell* head;
  cell* tail;
} queue;

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
queue init_queue() {
  queue q;
  q.head = NULL;
  q.tail = NULL;
  return q;
}

void release_queue(queue* q) {
  cell* current = q->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
}

/**
 * @brief 引数のqueue_headが指すcellを削除する.
 */
void clear_queue(queue* q) {
  q->head = NULL;
  q->tail = NULL;
}

/**
 * @brief queueが空か確認.
 */
bool empty_queue(queue* q) {
  return q->head == NULL;
}

/**
 * @brief キューの先頭にvalをエンキューする.
 */
void enter_queue(queue* q, int val) {
  cell* c = init_cell();
  c->element = val;
  c->next = NULL;

  if (empty_queue(q)) {
    q->head = c;
  } else {
    q->tail->next = c;
  }

  q->tail = c;
}

/**
 * @brief キューの先頭をデキューする.
 */
void remove_queue(queue* q, int* val) {
  if (empty_queue(q)) {
    printf("No more element can be dequeued from the queue.\n");
    return;
  }
  *val = q->head->element;
  cell* c = q->head;
  q->head = q->head->next;
  free(c);
}

/**
 * @brief 配列のデータがint型と前提にした,確認用プリント関数.
 */
void print_queue(queue q) {
  printf("QUEUE: [ ");
  cell* current = q.head;
  while (current != NULL) {
    printf("%d ", current->element);
    current = current->next;
  }
  printf("]\n");
}

int main() {
  queue q = init_queue();

  for (int i = 0; i < 10; i++) {
    enter_queue(&q, i);
  }
  print_queue(q);

  int val;
  remove_queue(&q, &val);
  print_queue(q);

  clear_queue(&q);
  print_queue(q);

  release_queue(&q);
  return 0;
}

// 実行結果
// QUEUE: [ 0 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ ]
