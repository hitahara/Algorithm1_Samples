#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief スタックの配列がとれる最大バイト数. */
#define MAX_ELEMENTS_MEMORY 65535

/**
 * @brief スタックの配列とメタ情報を保持する構造体.
 */
typedef struct stack {
  size_t sp;                                       /** 配列の長さ. */
  size_t size;                                     /** 配列がとれる最大長. */
  int elements[MAX_ELEMENTS_MEMORY / sizeof(int)]; /** int型で表された配列. */
} stack;

/**
 * @brief stackの初期化.
 * @return 初期化されたstackのポインタ.
 */
stack* init_stack() {
  stack* s = (stack*)malloc(sizeof(stack));
  s->sp = 0;
  s->size = MAX_ELEMENTS_MEMORY / sizeof(int);
  return s;
}

/**
 * @brief stackのspを0にする.
 * @param[in] stack spを0にするstackのポインタ.
 */
void clear_stack(stack* stack) {
  stack->sp = 0;
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
  if (stack->sp >= stack->size) {
    printf("No more element can be pushed into the stack.\n");
    return;
  }

  stack->elements[stack->sp] = val;
  stack->sp++;
}

/**
 * @brief スタックの先頭をポップし,stackを更新する.
 * @param[in] stack ポップする配列をもつstackのポインタ.
 * @param[in] val ポップした値の受けてとなるint型のポインタ.
 */
void pop(stack* stack, int* val) {
  if (stack->sp == 0) {
    printf("No more element can be popped from the elements.\n");
    return;
  }

  stack->sp--;
  *val = stack->elements[stack->sp];
}

/**
 * @brief stack確認用プリント関数.
 * @param[in] stack プリントする配列をもつstackのポインタ.
 */
void print_stack(stack* stack) {
  printf("ELEMENTS: [ ");
  for (int i = stack->sp - 1; i >= 0; i--) {
    printf("%d ", stack->elements[i]);
  }
  printf("]\nSTACK POINT: %ld\n", stack->sp);
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
  printf("val: %d\n", val);

  release_stack(stack);
  return 0;
}