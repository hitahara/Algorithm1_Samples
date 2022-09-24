#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 1000

typedef struct {
  int length;
  int elements[SIZE];
} stack;

void clear_stack(stack* stack) {
  stack->length = 0;
}

void push(stack* stack, int val) {
  if (stack->length >= SIZE) {
    printf("No more element can be pushed into the stack.\n");
    return;
  }

  stack->elements[stack->length] = val;
  stack->length++;
}

void pop(stack* stack, int* val) {
  if (stack->length == 0) {
    printf("No more element can be popped from the elements.\n");
    return;
  }

  stack->length--;
  *val = stack->elements[stack->length];
}

void print_stack(stack* s) {
  printf("ELEMENTS: [ ");
  for (int i = s->length - 1; i >= 0; i--) {
    printf("%d ", s->elements[i]);
  }
  printf("]\n");
  printf("STACK POINTER: %d\n", s->length);
}

int main() {
  stack s;
  s.length = 0;
  print_stack(&s);

  for (int i = 0; i < 10; i++) {
    push(&s, i);
  }
  print_stack(&s);

  int val;
  pop(&s, &val);
  print_stack(&s);
  printf("POP: %d\n", val);

  clear_stack(&s);
  print_stack(&s);

  return 0;
}

// 実行結果
// ELEMENTS: [ ]
// STACK POINTER: 0
// ELEMENTS: [ 9 8 7 6 5 4 3 2 1 0 ]
// STACK POINTER: 10
// ELEMENTS: [ 8 7 6 5 4 3 2 1 0 ]
// STACK POINTER: 9
// POP: 9
// ELEMENTS: [ ]
// STACK POINTER: 0
