#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _cell {
  int element;
  struct _cell* next;
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

void enter(queue* q, int val) {
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

// NOTE: `remove` は stdio.h で使われているので erase と書きます
void erase(queue* q, int* val) {
  if (empty(q)) {
    printf("No more element can be dequeued from the queue.\n");
    return;
  }
  *val = q->head->element;
  cell* c = q->head;
  q->head = q->head->next;
  free(c);
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
    enter(&q, i);
  }
  print(&q);

  int val;
  erase(&q, &val);
  print(&q);

  clear(&q);
  print(&q);
  return 0;
}

// 実行結果
// QUEUE: [ 0 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ ]
