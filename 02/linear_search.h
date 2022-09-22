#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int linear_search(int* table, int length, int x) {
  for (int i = 0; i < length; i++) {
    if (x == table[i]) {
      return true;
    }
  }

  return false;
}
