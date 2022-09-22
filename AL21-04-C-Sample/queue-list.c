#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief データと次のアイテムを保持する構造体.
*/
typedef struct cell
{
  int element; /** int型で表されたデータ. */
  struct cell* next;  /** 次のcellを指すポインタ. */
} cell;

/**
 * @brief cellのポインタの型.
*/
typedef cell* list;

/**
 * @brief cellの初期化.
 * @return 初期化された,データやポインタが代入されていないcellのポインタ.
*/
cell* init_cell()
{
  cell* c = (cell*) malloc( sizeof(cell) );
  return c;
}

/**
 * @brief listの初期化. 
 * @return NULLのlist.
*/
list init_list()
{
  list l = NULL;
  return l;
}

/**
 * @brief 引数のcellを削除する.
 * @param[in] c 削除するcellのポインタ.
*/
void dispose(cell* c)
{
  free(c);
}

/**
 * @brief 引数のlistをメモリ解放する.
 * @param[in] list_head メモリ解放するlistのポインタ.
*/
void release_list(list* list_head)
{
  cell* disposing_cell = *list_head;
  cell* next_cell = *list_head;
  for(;next_cell != NULL;)
  {
    next_cell = disposing_cell->next;
    dispose(disposing_cell);
    disposing_cell = next_cell;
  }
  *list_head = NULL;
}

/**
 * @brief 引数のqueue_headが指すcellを削除する.
 * @param[in] queue_head 削除するキューの先頭.
*/
void clear_queue(list* queue_head, list* queue_tail)
{
  release_list(queue_head);
  *queue_head = NULL;
  *queue_tail = NULL;
}

/**
 * @brief 引数のqueue_headが空か確認.
 * @param[in] queue_head 確認するキューの先頭.
*/
bool empty_queue(list queue_head)
{
  return queue_head == NULL;
}

/**
 * @brief キューの先頭にvalをエンキューし,queue_headとqueue_tailを更新する.
 * @param[in] queue_head キューの先頭を表すlistのポインタ.
 * @param[in] queue_tail キューの最後尾を表すlistのポインタ.
 * @param[in] val エンキューするint型のデータ.
*/
void enter_queue(list* queue_head, list* queue_tail, int val)
{
  cell* q = init_cell();
  q->element = val;
  q->next = NULL;

  if( empty_queue(*queue_head) )
  {
    *queue_head = q;
  }
  else
  {
    (*queue_tail)->next = q;
  }
  
  *queue_tail = q;
}


/**
 * @brief キューの先頭をデキューし,queue_headを更新する.
 * @param[in] queue_head 先頭をデキューするlistのポインタ.
 * @param[out] val デキューした値の受けてとなるint型のポインタ
*/
void remove_queue(list* queue_head, int* val)
{
  if( empty_queue(*queue_head) )
  {
    printf("No more element can be dequeued from the queue.\n");
    return;
  }
  *val = (*queue_head)->element;
  cell* q = *queue_head;
  *queue_head = (*queue_head)->next;
  dispose(q);
}

/**
 * @brief 配列のデータがint型と前提にした,確認用プリント関数.
 * @param[in] list プリントするlist.
*/
void print_list(list list)
{
  cell* c = list;
  printf("LIST CELLS: [ ");
  for(;c != NULL;c = c->next)
  {
    printf("%d ", c->element);   
  }
  printf("]\n");
}

int main()
{
  list queue_head = init_list();
  list queue_tail = init_list(); 

  for(int i = 0; i < 20; i++)
  {
    enter_queue(&queue_head, &queue_tail, i);
  }
  
  print_list(queue_head);

  int val;
  remove_queue(&queue_head, &val);

  print_list(queue_head);


  clear_queue(&queue_head, &queue_tail);
  return 0;
}