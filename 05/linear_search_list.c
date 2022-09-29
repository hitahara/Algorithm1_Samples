#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int* a, int* b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

// Fisher–Yates shuffle というアルゴリズムを使います。
void shuffle(int* array, int length) {
  int i = length;
  while (i > 1) {
    int j = rand() % i--;
    swap(&array[i], &array[j]);
  }
}

typedef struct _record {
  int key;
  char value[32];
  struct _record* next;
} record;

typedef struct {
  record* header;
  record* sentinel;
} table;

record* init_record(int key, const char* value) {
  record* rec = (record*)malloc(sizeof(record));
  rec->next = NULL;
  rec->key = key;
  strcpy(rec->value, value);
  return rec;
}

void clear(table* tab) {
  record* current = tab->header->next;
  while (current != tab->sentinel) {
    record* next = current->next;
    free(current);
    current = next;
  }

  free(tab->header);
  free(tab->sentinel);
  tab->header = NULL;
  tab->sentinel = NULL;
}

void insert_head(table* tab, record* p) {
  p->next = tab->header->next;
  tab->header->next = p;
}

record* search_previous(table* tab, int target) {
  tab->sentinel->key = target;
  record* previous = tab->header;
  record* current = tab->header->next;
  while (target != current->key) {
    previous = current;
    current = current->next;
  }
  bool found = current != tab->sentinel;

  // コメントアウトを外すと自己再構成リストになります。
  // if (found) {
  //   previous->next = current->next;
  //   insert_head(tab, current);
  // }
  return found ? previous : NULL;
}

void erase_target(table* tab, int target) {
  record* previous = search_previous(tab, target);
  record* current = previous->next;
  previous->next = current->next;
  free(current);
}

void cli_insert_head(table* tab) {
  printf("Type in a key (>= 0) and a field. (example: \"100 BBB\")\n");
  while (true) {
    record* rec = init_record(-1, "");
    scanf("%d %s", &rec->key, rec->value);

    if (search_previous(tab, rec->key) != NULL) {
      printf("The key is already used.\n");
    } else {
      insert_head(tab, rec);
      return;
    }
  }
}

void print(table* tab) {
  printf("TABLE: [ ");
  int length = 0;
  record* current = tab->header->next;
  while (current != tab->sentinel) {
    printf("{%d, %s} ", current->key, current->value);
    current = current->next;
    length++;
  }
  printf("]\n");
  printf("LENGTH: %d\n\n", length);
}

int main() {
  // shuffle keys
  int keys[] = {0, 1, 2, 3, 4};
  int num_keys = sizeof(keys) / sizeof(int);
  shuffle(keys, num_keys);

  // create table
  table tab;
  tab.sentinel = init_record(-1, "");
  tab.header = init_record(-1, "");
  tab.header->next = tab.sentinel;
  for (int i = 0; i < num_keys; i++) {
    record* rec = init_record(keys[i], "AAA");
    insert_head(&tab, rec);
  }
  print(&tab);

  // insert user input
  cli_insert_head(&tab);
  print(&tab);

  // search 3
  record* previous = search_previous(&tab, 3);
  bool found = previous != NULL;
  printf("3 was %s\n", found ? previous->next->value : "NOT FOUND.");
  print(&tab);

  // erase 3
  erase_target(&tab, 3);
  print(&tab);

  // search 3
  previous = search_previous(&tab, 3);
  found = previous != NULL;
  printf("3 was %s\n", found ? previous->next->value : "NOT FOUND.");

  clear(&tab);
  return 0;
}

// 実行結果
// TABLE: [ {3, AAA} {2, AAA} {0, AAA} {1, AAA} {4, AAA} ]
// LENGTH: 5
//
// Type in a key (>= 0) and a field. (example: "100 BBB")
// 100 B
// TABLE: [ {100, B} {3, AAA} {2, AAA} {0, AAA} {1, AAA} {4, AAA} ]
// LENGTH: 6
//
// 3 was AAA
// TABLE: [ {100, B} {3, AAA} {2, AAA} {0, AAA} {1, AAA} {4, AAA} ]
// LENGTH: 6
//
// TABLE: [ {100, B} {2, AAA} {0, AAA} {1, AAA} {4, AAA} ]
// LENGTH: 5
//
// 3 was NOT FOUND.
