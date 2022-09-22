#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/** 
 * @def MAX_ELEMENTS_MEMORY
 * @brief queueの配列がとれる最大バイト数. 
*/
#define MAX_ELEMENTS_MEMORY 65535

/**
 * @brief キューの配列とメタ情報を保持する構造体.
*/
typedef struct queue
{
  size_t head;                                     /** キューの先頭のインデックス. */
  size_t tail;                                     /** キューの最後尾のインデックス. */
  size_t count;                                    /** 配列の長さ. */
  size_t size;                                     /** 配列がとれる最大長. */
  int elements[MAX_ELEMENTS_MEMORY / sizeof(int)]; /** int型で表された配列. */
} queue;

/**
 * @brief queueの初期化.
 * @return 初期化されたqueueのポインタ.
*/
queue* init_queue()
{
  queue* s = (queue*) malloc( sizeof(queue) );  
  s->head = 0;
  s->tail = 0;
  s->count = 0;
  s->size = MAX_ELEMENTS_MEMORY / sizeof(int);
  return s;
}

/**
 * @brief 引数のqueueのメタデータを0にする.
 * @param[in] queue 削除するqueueのポインタ.
*/
void clear_queue(queue* queue)
{
  queue->head = 0;
  queue->tail = 0;
  queue->count = 0;
}

/**
 * @brief 引数のqueueが空か確認.
 * @param[in] queue 確認するqueueのポインタ.
*/
bool empty_queue(queue* queue)
{
  return queue->count == 0;
}

/**
 * @brief queueのメモリを解放.
 * @param[in] queue メモリを解放するqueueのポインタ.
*/
void release_queue(queue* queue)
{
  free(queue);
}

/**
 * @brief キューの先頭にvalをエンキューし,queueを更新する.
 * @param[in] queue 先頭にエンキューするqueueのポインタ.
 * @param[in] val エンキューするint型のデータ.
*/
void enter_queue(queue* queue, int val)
{
  if(queue->count >= queue->size)
  {
    printf("No more element can be enqueued into the queue.\n");
    return;
  }

  queue->elements[queue->tail] = val;
  queue->tail++;

  if(queue->tail >= queue->size)
  {
    queue->tail = 0;
  }

  queue->count++;
}

/**
 * @brief キューの先頭をデキューし,stackを更新する.
 * @param[in] queue 先頭をデキューするqueueのポインタ.
 * @param[out] val デキューした値の受けてとなるint型のポインタ
*/
void remove_queue(queue* queue, int* val)
{
  if(queue->count <= 0)
  {
    printf("No more element can be dequeued from the queue.\n");
    return;
  }
  
  *val = queue->elements[queue->head];
  queue->head++;

  if(queue->head >= queue->size)
  {
    queue->head = 0;
  }

  queue->count--;
}

/**
 * @brief queue確認用プリント関数.
 * @param[in] queue プリントする配列をもつqueueのポインタ.
*/
void print_queue(queue* queue)
{
  printf("QUEUE: [ ");
  //int jはqueue->count==queue->sizeの時
  //queue->head==queue->tailとなるため存在
  for(int i = queue->head, j = 0; j < queue->count; i = (i+1) % queue->size, j++ )
  {
    printf("%d ", queue->elements[i] );
  }
  printf("]\n");
  /* MAX_ELEMENTS_MEMORYを sizeof(int) * 8 = 32等にして
    操作の内容を変更し
    リングバッファの様子を確認してみよう.
  printf("ELEMENTS: [ ");
  for(int i = 0; i < queue->size; i++ )
  {
    printf("%d ", queue->elements[i] );
  }
  printf("]\n");
  */
}

int main()
{
  queue* queue = init_queue();

  for(int i = 0; i < 20; i++)
  {
    enter_queue(queue, i);
  }

  print_queue(queue);

  int val;
  remove_queue(queue, &val);

  print_queue(queue);

  release_queue(queue);
  return 0;
}