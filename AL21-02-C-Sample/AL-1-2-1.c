#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int linear_search(int *table, int table_length, int x)
{
  bool found = false;

  // 0 origin
  for (int i = 0; i < table_length; i++)
  {
    if (x == table[i])
    {
      found = true;
      break;
    }
  }

  return found;
}

int linear_search_analyze(int *table, int table_length, int x)
{
  bool found = false; // 1

  // 0 origin
  int i = 0;               // 2
  while (i < table_length) // 3
  {
    if (x != table[i]) // 4
    {
      i = i + 1; // 7
    }
    else
    {
      found = true; // 5
      break;        // 6
    }
  }

  return found;
}

int main()
{
  double start_clock, end_clock;

  int *table;
  int table_length;
  int target = 1;

  printf("Type in the length of the array...\n");
  scanf("%d", &table_length);

  table = (int *)malloc(table_length * sizeof(int));

  printf("Type in the table numbers (number >= 0)...\n");
  for (int i = 0; i < table_length; i++)
  {
    scanf("%d", &table[i]);
  }

  while (true)
  {
    printf("\nType in the target number... (Type in number < 0 to exit)\n");
    scanf("%d", &target);

    if (target < 0)
    {
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
