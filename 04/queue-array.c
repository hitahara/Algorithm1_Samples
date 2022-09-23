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
  queue* s = (queue*)malloc(sizeof(queue));
  s->head = 0;
  s->tail = 0;
  s->count = 0;
  return s;
}

/**
 * @brief 引数のqueueのメタデータを0にする.
 */
void clear_queue(queue* queue) {
  queue->head = 0;
  queue->tail = 0;
  queue->count = 0;
}

/**
 * @brief 引数のqueueが空か確認.
 */
bool empty_queue(queue* queue) {
  return queue->count == 0;
}

/**
 * @brief queueのメモリを解放.
 */
void release_queue(queue* queue) {
  free(queue);
}

/**
 * @brief キューの先頭にvalをエンキューし,queueを更新する.
 */
void enter_queue(queue* queue, int val) {
  if (queue->count >= SIZE) {
    printf("No more element can be enqueued into the queue.\n");
    return;
  }

  queue->elements[queue->tail] = val;
  queue->tail++;

  if (queue->tail >= SIZE) {
    queue->tail = 0;
  }

  queue->count++;
}

/**
 * @brief キューの先頭をデキューする.
 */
void remove_queue(queue* queue, int* val) {
  if (queue->count <= 0) {
    printf("No more element can be dequeued from the queue.\n");
    return;
  }

  *val = queue->elements[queue->head];
  queue->head++;

  if (queue->head >= SIZE) {
    queue->head = 0;
  }

  queue->count--;
}

/**
 * @brief queue確認用プリント関数.
 * @param[in] queue プリントする配列をもつqueueのポインタ.
 */
void print_queue(queue* queue) {
  printf("QUEUE: [ ");
  // int jはqueue->count==SIZEの時
  // queue->head==queue->tailとなるため存在
  int index = queue->head;
  for (int count = 0; count < queue->count; count++) {
    printf("%d ", queue->elements[index]);
    index = (index + 1) % SIZE;
  }
  printf("]\n");
  /* SIZEを8等にして操作の内容を変更し
    リングバッファの様子を確認してみよう.
  printf("ELEMENTS: [ ");
  for(int i = 0; i < SIZE; i++ )
  {
    printf("%d ", queue->elements[i] );
  }
  printf("]\n");
  */
}

int main() {
  queue* queue = init_queue();

  for (int i = 0; i < 10; i++) {
    enter_queue(queue, i);
  }
  print_queue(queue);

  int val;
  remove_queue(queue, &val);
  print_queue(queue);

  release_queue(queue);
  return 0;
}

// 実行結果
// QUEUE: [ 0 1 2 3 4 5 6 7 8 9 ]
// QUEUE: [ 1 2 3 4 5 6 7 8 9 ]
