#include <stdio.h>
#include <stdlib.h>

#define SIZE 1000

// NOTE:
// C 言語では、struct name {}; と定義してしまうと変数宣言の
// 際に毎回 struct name var; と書く必要があります。
// typedef struct {} name; と書くことで name var; と簡潔に
// 書くことができます。
typedef struct {
  int length;
  int elements[SIZE];
} sequence;

// NOTE:
// 2 つの理由から関数には構造体のポインタを渡しています。
// - 1 つめの理由は関数内で構造体を変更するためです。
//   構造体自体を渡してしまうと、構造体のコピーが発生するため
//   関数呼び出し側が渡したインスタンスに影響を与えられません。
// - 2 つめの理由はパフォーマンスを改善するためです。
//   構造体自体を渡してしまうと、構造体のコピーが発生するため
//   低速です。CPU の演算速度に比べるとメモリアクセスは超低速です
//   ので注意が必要です。ポインタを渡すと構造体のコピーが発生せず
//   ポインタのコピーが発生します。
//   sequence のサイズは sizeof(int) * (1 + SIZE) ですので
//   大抵の処理系では 4004 bytes です。それに対してポインタは
//   8 bytes と圧倒的に小さいです。
void insert(sequence* seq, int pos, int val) {
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

// NOTE: `delete` は C++ の予約語なので、念のため erase と書きます
void erase(sequence* seq, int pos) {
  seq->length--;
  for (int i = pos; i < seq->length; i++) {
    seq->elements[i] = seq->elements[i + 1];
  }
}

void print(sequence* seq) {
  printf("ELEMENTS: [ ");
  for (int i = 0; i < seq->length; i++) {
    printf("%d ", seq->elements[i]);
  }
  printf("]\n");
  printf("LENGTH  : %d\n", seq->length);
}

int main() {
  sequence seq = {0, {0}};

  for (int i = 0; i < 10; i++) {
    insert(&seq, i, i);
  }
  print(&seq);

  erase(&seq, 5);
  print(&seq);

  return 0;
}

// 実行結果
// ELEMENTS: [ 0 1 2 3 4 5 6 7 8 9 ]
// LENGTH  : 10
// ELEMENTS: [ 0 1 2 3 4 6 7 8 9 ]
// LENGTH  : 9
