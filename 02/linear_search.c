#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int linear_search(int* table, int table_length, int x) {
  for (int i = 0; i < table_length; i++) {
    if (x == table[i]) {
      return true;
    }
  }

  return false;
}

int linear_search_analyze(int* table, int table_length, int x) {
  int i = 0;
  while (i < table_length) {
    if (x != table[i]) {
      i = i + 1;
    } else {
      return true;
    }
  }

  return false;
}

int main() {
  double start_clock, end_clock;

  int* table;
  int table_length;
  int target = 1;

  printf("Type in the length of the array...\n");
  scanf("%d", &table_length);

  table = (int*)malloc(table_length * sizeof(int));

  printf("Type in the table numbers (number >= 0)...\n");
  for (int i = 0; i < table_length; i++) {
    scanf("%d", &table[i]);
  }

  while (true) {
    printf("\nType in the target number... (Type in number < 0 to exit)\n");
    scanf("%d", &target);

    if (target < 0) {
      break;
    }

    start_clock = (double)clock();
    bool search_result = linear_search(table, table_length, target);
    end_clock = (double)clock();

    printf("CPU execution time: %.8lf s.\nIs target in the table: %s\n",
           (end_clock - start_clock) / CLOCKS_PER_SEC, search_result ? "Yes" : "No");
  }

  free(table);

  return 0;
}
