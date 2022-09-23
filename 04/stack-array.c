#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief スタックの配列がとれる最大要素数. */
#define SIZE 1000

/**
 * @brief スタックの配列とメタ情報を保持する構造体.
 */
typedef struct {
  size_t pointer;     /** 配列の長さ. */
  int elements[SIZE]; /** int型で表された配列. */
} stack;

/**
 * @brief stackの初期化.
 * @return 初期化されたstackのポインタ.
 */
stack* init_stack() {
  stack* s = (stack*)malloc(sizeof(stack));
  s->pointer = 0;
  return s;
}

/**
 * @brief stackのspを0にする.
 * @param[in] stack spを0にするstackのポインタ.
 */
void clear_stack(stack* stack) {
  stack->pointer = 0;
}

/**
 * @brief stackのメモリを解放.
 * @param[in] stack メモリを解放するstackのポインタ.
 */
void release_stack(stack* stack) {
  free(stack);
}

/**
 * @brief valをスタックにプッシュする.
 * @param[in] stack プッシュする配列をもつstackのポインタ.
 * @param[in] val int型のプッシュするデータ.
 */
void push(stack* stack, int val) {
  if (stack->pointer >= SIZE) {
    printf("No more element can be pushed into the stack.\n");
    return;
  }

  stack->elements[stack->pointer] = val;
  stack->pointer++;
}

/**
 * @brief スタックの先頭をポップし,stackを更新する.
 * @param[in] stack ポップする配列をもつstackのポインタ.
 * @param[in] val ポップした値の受けてとなるint型のポインタ.
 */
void pop(stack* stack, int* val) {
  if (stack->pointer == 0) {
    printf("No more element can be popped from the elements.\n");
    return;
  }

  stack->pointer--;
  *val = stack->elements[stack->pointer];
}

/**
 * @brief stack確認用プリント関数.
 * @param[in] stack プリントする配列をもつstackのポインタ.
 */
void print_stack(stack* stack) {
  printf("ELEMENTS: [ ");
  for (int i = stack->pointer - 1; i >= 0; i--) {
    printf("%d ", stack->elements[i]);
  }
  printf("]\nSTACK POINTER: %ld\n", stack->pointer);
}

int main() {
  stack* stack = init_stack();
  print_stack(stack);

  for (int i = 0; i < 10; i++) {
    push(stack, i);
  }
  print_stack(stack);

  int val;
  pop(stack, &val);
  print_stack(stack);
  printf("POP: %d\n", val);

  release_stack(stack);
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
