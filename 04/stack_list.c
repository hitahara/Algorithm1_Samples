#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _cell {
  int element;
  struct _cell* next;
} cell;

typedef struct
{
  cell* head;
} stack;

void clear(stack* s) {
  cell* current = s->head;
  while (current != NULL) {
    cell* next = current->next;
    free(current);
    current = next;
  }
  s->head = NULL;
}

void push(stack* s, int val) {
  cell* c = (cell*)malloc(sizeof(cell));
  c->element = val;
  c->next = s->head;
  s->head = c;
}

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

void print(stack s) {
  printf("LIST CELLS: [ ");
  cell* c = s.head;
  while (c != NULL) {
    printf("%d ", c->element);
    c = c->next;
  }
  printf("]\n");
}

int main() {
  stack s = {NULL};

  for (int i = 0; i < 10; i++) {
    push(&s, i);
  }
  print(s);

  int val;
  pop(&s, &val);
  print(s);
  printf("POP: %d\n", val);

  clear(&s);
  return 0;
}

// 実行結果
// LIST CELLS: [ 9 8 7 6 5 4 3 2 1 0 ]
// LIST CELLS: [ 8 7 6 5 4 3 2 1 0 ]
// POP: 9
