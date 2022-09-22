#include "binary_search.h"
#include "linear_search.h"

int main() {
  int length;
  printf("Type in the length of the array...\n");
  scanf("%d", &length);

  int* table = (int*)malloc(length * sizeof(int));

  printf("Type in the table numbers (number >= 0)...\n");
  for (int i = 0; i < length; i++) {
    scanf("%d", &table[i]);
  }

  while (true) {
    int target = -1;
    printf("\nType in the target number... (Type in number < 0 to exit)\n");
    scanf("%d", &target);
    if (target < 0) {
      break;
    }

    double start_clock = (double)clock();
    bool found = linear_search(table, length, target);
    // bool found = binary_search(table, length, target);
    double end_clock = (double)clock();

    printf("CPU execution time: %.8lf s.\n", (end_clock - start_clock) / CLOCKS_PER_SEC);
    printf("Is target in the table: %s\n", found ? "Yes" : "No");
  }

  free(table);

  return 0;
}
