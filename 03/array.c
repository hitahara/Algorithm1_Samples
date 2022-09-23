#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief sequenceの最大要素数.
 */
#define SIZE 1000

/**
 * @brief 配列とそのメタデータを保持する構造体.
 */
typedef struct {
  size_t length;      /** 配列の長さ. */
  int elements[SIZE]; /** int型で表された配列. */
} sequence;

/**
 * @brief sequenceの初期化.
 */
sequence* init_sequence() {
  sequence* s = (sequence*)malloc(sizeof(sequence));
  s->length = 0;
  return s;
}

/**
 * @brief sequenceのメモリを解放.
 */
void release_sequence(sequence* seq) {
  free(seq);
}

/**
 * @brief 配列の指定位置にデータを挿入し,指定位置以降を後ろにずらす. Program 1.3.1
 */
void insert_element(sequence* seq, int pos, int val) {
  if (seq->length >= SIZE) {
    printf("No more element can be inserted into elements.\n");
    return;
  }

  for (int i = seq->length - 1; i >= pos; i--) {
    seq->elements[i + 1] = seq->elements[i];
  }

  seq->length++;
  seq->elements[pos] = val;
}

/**
 * @brief 配列の指定位置のデータを削除し,指定位置以降を前にずらす. Program 1.3.2
 */
void delete_element(sequence* seq, int pos) {
  seq->length--;

  for (int i = pos; i < seq->length; i++) {
    seq->elements[i] = seq->elements[i + 1];
  }
}

/**
 * @brief sequence確認用プリント関数.
 */
void print_sequence(sequence* seq) {
  printf("ELEMENTS: [ ");
  for (size_t i = 0; i < seq->length; i++) {
    printf("%d ", seq->elements[i]);
  }
  printf("]\nLENGTH  : %ld\n", seq->length);
}

int main() {
  sequence* array = init_sequence();

  for (int i = 0; i < 10; i++) {
    insert_element(array, i, i);
  }
  print_sequence(array);

  delete_element(array, 5);
  print_sequence(array);

  release_sequence(array);
  return 0;
}

// 実行結果
// ELEMENTS: [ 0 1 2 3 4 5 6 7 8 9 ]
// LENGTH  : 10
// ELEMENTS: [ 0 1 2 3 4 6 7 8 9 ]
// LENGTH  : 9
