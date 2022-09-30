#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1000

typedef struct {
  int length;
  int elements[SIZE];
} stack;

void clear(stack* stack) {
  stack->length = 0;
}

void push(stack* stack, int val) {
  assert(stack->length < SIZE);

  stack->elements[stack->length] = val;
  stack->length++;
}

void pop(stack* stack, int* val) {
  assert(stack->length != 0);

  stack->length--;
  *val = stack->elements[stack->length];
}

void print(stack* s) {
  printf("ELEMENTS: [ ");
  for (int i = s->length - 1; i >= 0; i--) {
    printf("%d ", s->elements[i]);
  }
  printf("]\n");
  printf("STACK POINTER: %d\n", s->length);
}

int main() {
  stack s = {0, {0}};

  for (int i = 0; i < 10; i++) {
    push(&s, i);
  }
  print(&s);

  int val;
  pop(&s, &val);
  print(&s);
  printf("POP: %d\n", val);

  clear(&s);
  print(&s);

  return 0;
}

// 実行結果
// ELEMENTS: [ 9 8 7 6 5 4 3 2 1 0 ]
// STACK POINTER: 10
// ELEMENTS: [ 8 7 6 5 4 3 2 1 0 ]
// STACK POINTER: 9
// POP: 9
// ELEMENTS: [ ]
// STACK POINTER: 0
