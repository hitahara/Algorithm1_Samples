#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cell_ {
  int element;
  struct cell_* next;
} cell;

typedef struct {
  cell* head;
  cell* tail;
} queue;

void clear(queue* q) {
  cell* current = q->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
  q->head = NULL;
  q->tail = NULL;
}

bool empty(queue* q) {
  return q->head == NULL;
}

void enqueue(queue* q, int val) {
  cell* c = (cell*)malloc(sizeof(cell));
  c->element = val;
  c->next = NULL;

  if (empty(q)) {
    q->head = c;
  } else {
    q->tail->next = c;
  }

  q->tail = c;
}

// 講義スライドと異なり、dequeue された値を返り値とします。
int dequeue(queue* q) {
  assert(!empty(q));

  cell* c = q->head;
  int val = c->element;
  q->head = q->head->next;
  free(c);
  return val;
}

void print(queue* q) {
  printf("QUEUE: [ ");
  cell* current = q->head;
  while (current != NULL) {
    printf("%d ", current->element);
    current = current->next;
  }
  printf("]\n");
}

int main() {
  queue q = {NULL, NULL};

  for (int i = 0; i < 10; i++) {
    enqueue(&q, i);
  }
  print(&q);

  printf("DEQUEUE: %d\n", dequeue(&q));
  print(&q);

  clear(&q);
  print(&q);
  return 0;
}

// 実行結果
// QUEUE: [ 0 1 2 3 4 5 6 7 8 9 ]
// DEQUEUE: 0
// QUEUE: [ 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ ]
