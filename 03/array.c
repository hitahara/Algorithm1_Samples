#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def MAX_ELEMENTS_MEMORY
 * @brief sequenceの配列がとれる最大バイト数.
 */
#define MAX_ELEMENTS_MEMORY 65535

/**
 * @brief 配列とそのメタデータを保持する構造体.
 */
typedef struct sequence {
  size_t elements_length;                          /** 配列の長さ. */
  size_t size;                                     /** 配列がとれる最大長. */
  int elements[MAX_ELEMENTS_MEMORY / sizeof(int)]; /** int型で表された配列. */
} sequence;

/**
 * @brief sequenceの初期化.
 * @return 初期化されたsequenceのポインタ.
 */
sequence* init_sequence() {
  sequence* s = (sequence*)malloc(sizeof(sequence));
  s->elements_length = 0;
  s->size = MAX_ELEMENTS_MEMORY / sizeof(int);
  return s;
}

/**
 * @brief sequenceのメモリを解放.
 * @param[in] seq メモリを開放するsequence.
 */
void release_sequence(sequence* seq) {
  free(seq);
}

/**
 * @brief 配列の指定位置にデータを挿入し,指定位置以降を後ろにずらす. Program 1.3.1
 * @param[in] seq 挿入対象の配列をもつsequenceのポインタ.
 * @param[in] k   挿入する位置/キー/インデックス (0 <= k <= seq->elements_lengthを前提).
 * @param[in] val int型の挿入データ.
 */
void insert_element(sequence* seq, int k, int val) {
  if (seq->elements_length >= seq->size) {
    printf("No more element can be inserted into elements.\n");
    return;
  }

  for (int i = seq->elements_length - 1; i >= k; i--) {
    seq->elements[i + 1] = seq->elements[i];
  }

  seq->elements_length++;
  seq->elements[k] = val;
}

/**
 * @brief 配列の指定位置のデータを削除し,指定位置以降を前にずらす. Program 1.3.2
 * @param[in] seq 削除対象の配列をもつsequenceのポインタ.
 * @param[in] k   削除する位置/キー/インデックス (0 <= k < seq->elements_lengthを前提).
 */
void delete_element(sequence* seq, int k) {
  seq->elements_length--;

  for (size_t i = k; i < seq->elements_length; i++) {
    seq->elements[i] = seq->elements[i + 1];
  }
}

/**
 * @brief sequence確認用プリント関数.
 * @param[in] seq プリントする配列をもつsequenceのポインタ.
 */
void print_sequence(sequence* seq) {
  printf("ELEMENTS: [ ");
  for (size_t i = 0; i < seq->elements_length; i++) {
    printf("%d ", seq->elements[i]);
  }
  printf("]\nLENGTH  : %ld\n", seq->elements_length);
}

int main() {
  sequence* array = init_sequence();

  for (int i = 0; i < 20; i++) {
    insert_element(array, i, i);
  }
  print_sequence(array);
  delete_element(array, 5);
  print_sequence(array);

  release_sequence(array);
  return 0;
}