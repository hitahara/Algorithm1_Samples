#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int binary_search(int* table, int table_length, int x) {
  int lo = 0;
  int hi = table_length - 1;
  int mid;

  while (lo <= hi) {
    mid = (lo + hi) / 2;
    if (x < table[mid]) {
      hi = mid - 1;
    } else {
      lo = mid + 1;
    }
  }

  return (hi >= 0) && (x == table[hi]);
}
