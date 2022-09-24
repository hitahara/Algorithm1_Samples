#include <stdio.h>
#include <stdlib.h>

#define SIZE 1000

typedef struct {
  int length;
  int elements[SIZE];
} sequence;

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

void delete_element(sequence* seq, int pos) {
  seq->length--;

  for (int i = pos; i < seq->length; i++) {
    seq->elements[i] = seq->elements[i + 1];
  }
}

void print_sequence(sequence* seq) {
  printf("ELEMENTS: [ ");
  for (int i = 0; i < seq->length; i++) {
    printf("%d ", seq->elements[i]);
  }
  printf("]\nLENGTH  : %d\n", seq->length);
}

int main() {
  sequence array;
  array.length = 0;

  for (int i = 0; i < 10; i++) {
    insert_element(&array, i, i);
  }
  print_sequence(&array);

  delete_element(&array, 5);
  print_sequence(&array);

  return 0;
}

// 実行結果
// ELEMENTS: [ 0 1 2 3 4 5 6 7 8 9 ]
// LENGTH  : 10
// ELEMENTS: [ 0 1 2 3 4 6 7 8 9 ]
// LENGTH  : 9
