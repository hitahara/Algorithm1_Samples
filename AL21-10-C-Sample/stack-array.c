#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief スタックの配列がとれる最大バイト数. */
#define MAX_STACK_SIZE 40

typedef struct stack_data stack_data;
typedef struct stack stack;

stack *init_stack();
void release_stack(stack **stack);
void push_stack(stack *stack, size_t left, size_t right);
void pop_stack(stack *stack, size_t *left, size_t *right);

/**
 * @brief スタックの配列とメタ情報を保持する構造体.
 */
struct stack_data
{
  size_t left;
  size_t right;
};

/**
 * @brief スタックの配列とメタ情報を保持する構造体.
 */
struct stack
{
  size_t sp;                           /** 配列の長さ. */
  size_t size;                         /** 配列がとれる最大長. */
  stack_data elements[MAX_STACK_SIZE]; /** int型で表された配列. */
};

/**
 * @brief stackの初期化.
 * @return 初期化されたstackのポインタ.
 */
stack *init_stack()
{
  stack *s = (stack *)malloc(sizeof(stack));
  s->sp = 0;
  s->size = MAX_STACK_SIZE;
  return s;
}

/**
 * @brief stackのメモリを解放.
 * @param[in] stack メモリを解放するstackのポインタ.
 */
void release_stack(stack **stack)
{
  (*stack)->sp = 0;
  free(*stack);
  *stack = NULL;
}

/**
 * @brief valをスタックにプッシュする.
 * @param[in] stack プッシュする配列をもつstackのポインタ.
 * @param[in] data stack_data型のプッシュするデータ.
 */
void push_stack(stack *stack, size_t left, size_t right)
{
  if (stack->sp >= stack->size)
  {
    printf("No more element can be pushed into the stack.\n");
    return;
  }

  stack->elements[stack->sp].left = left;
  stack->elements[stack->sp].right = right;
  stack->sp++;
}

/**
 * @brief スタックの先頭をポップし,stackを更新する.
 * @param[in] stack ポップする配列をもつstackのポインタ.
 * @param[in] data ポップした値の受けてとなるstack_data型のポインタ.
 */
void pop_stack(stack *stack, size_t *left, size_t *right)
{
  if (stack->sp == 0)
  {
    printf("No more element can be popped from the elements.\n");
    return;
  }

  stack->sp--;
  *left = stack->elements[stack->sp].left;
  *right = stack->elements[stack->sp].right;
}

int main()
{
  return 0;
}