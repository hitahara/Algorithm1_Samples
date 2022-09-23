#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief queueの配列がとれる最大要素数.
 */
#define SIZE 1000

/**
 * @brief キューの配列とメタ情報を保持する構造体.
 */
typedef struct {
  size_t head;        /** キューの先頭のインデックス. */
  size_t tail;        /** キューの最後尾のインデックス. */
  size_t count;       /** 配列の長さ. */
  int elements[SIZE]; /** int型で表された配列. */
} queue;

/**
 * @brief queueの初期化.
 */
queue* init_queue() {
  queue* q = (queue*)malloc(sizeof(queue));
  q->head = 0;
  q->tail = 0;
  q->count = 0;
  return q;
}

/**
 * @brief 引数のqueueのメタデータを0にする.
 */
void clear_queue(queue* q) {
  q->head = 0;
  q->tail = 0;
  q->count = 0;
}

/**
 * @brief 引数のqueueが空か確認.
 */
bool empty_queue(queue* q) {
  return q->count == 0;
}

/**
 * @brief queueのメモリを解放.
 */
void release_queue(queue* q) {
  free(q);
}

/**
 * @brief キューの先頭にvalをエンキューし,queueを更新する.
 */
void enter_queue(queue* q, int val) {
  if (q->count >= SIZE) {
    printf("No more element can be enqueued into the queue.\n");
    return;
  }

  q->elements[q->tail] = val;
  q->tail++;

  if (q->tail >= SIZE) {
    q->tail = 0;
  }

  q->count++;
}

/**
 * @brief キューの先頭をデキューする.
 */
void remove_queue(queue* q, int* val) {
  if (q->count <= 0) {
    printf("No more element can be dequeued from the queue.\n");
    return;
  }

  *val = q->elements[q->head];
  q->head++;

  if (q->head >= SIZE) {
    q->head = 0;
  }

  q->count--;
}

/**
 * @brief queue確認用プリント関数.
 */
void print_queue(queue* q) {
  printf("QUEUE: [ ");
  int index = q->head;
  for (int count = 0; count < q->count; count++) {
    printf("%d ", q->elements[index]);
    index = (index + 1) % SIZE;
  }
  printf("]\n");

  // SIZEを8等にして操作の内容を変更し
  // リングバッファの様子を確認してみよう.
  // printf("ELEMENTS: [ ");
  // for(int i = 0; i < SIZE; i++ )
  // {
  //   printf("%d ", q->elements[i] );
  // }
  // printf("]\n");
}

int main() {
  queue* q = init_queue();

  for (int i = 0; i < 10; i++) {
    enter_queue(q, i);
  }
  print_queue(q);

  int val;
  remove_queue(q, &val);
  print_queue(q);

  release_queue(q);
  return 0;
}

// 実行結果
// QUEUE: [ 0 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ 1 2 3 4 5 6 7 8 9 ]
