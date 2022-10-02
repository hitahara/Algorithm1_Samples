#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_RECORDS 1000

typedef struct {
  int key;
  char value[32];
} record;

typedef struct {
  int length;
  record records[MAX_NUM_RECORDS];
} table;

// target と同じ key を持つ record の index を返します。
// 条件に合致する record が見つからない場合は -1 を返します。
int search(table* tab, int target) {
  tab->records[tab->length].key = target;  // sentinel
  int index = 0;
  while (target != tab->records[index].key) {
    index++;
  }
  return index < tab->length ? index : -1;
}

void insert(table* tab, record rec) {
  assert(tab->length < MAX_NUM_RECORDS - 1);

  tab->records[tab->length] = rec;
  tab->length++;
}

void erase(table* tab, int pos) {
  for (int i = pos; i < tab->length - 1; i++) {
    tab->records[i] = tab->records[i + 1];
  }
  tab->length--;
}

void cli_insert(table* tab) {
  printf("Type in a key (>= 0) and a field. (example: \"100 BBB\")\n");
  while (true) {
    record rec;
    scanf("%d %s", &rec.key, rec.value);
    if (search(tab, rec.key) != -1) {
      printf("The key is already used.\n");
    } else {
      insert(tab, rec);
      return;
    }
  }
}

void print(table* tab) {
  printf("TABLE: [ ");
  for (int i = 0; i < tab->length; i++) {
    printf("{%d, %s} ", tab->records[i].key, tab->records[i].value);
  }
  printf("]\n");
  printf("LENGTH: %d\n\n", tab->length);
}

int main() {
  // create table
  table tab = {0};
  int keys[] = {4, 1, 0, 2, 3};
  int num_keys = sizeof(keys) / sizeof(int);
  for (int i = 0; i < num_keys; i++) {
    record rec = {keys[i], "AAA"};
    insert(&tab, rec);
  }
  print(&tab);

  // insert user input
  cli_insert(&tab);
  print(&tab);

  // search 3
  int target = 3;
  int index = search(&tab, target);
  if (index != -1) {
    printf("%d was %s\n", target, tab.records[index].value);
  } else {
    printf("%d was NOT FOUND\n", target);
  }

  // erase 3
  erase(&tab, index);
  print(&tab);

  // search 3
  index = search(&tab, target);
  if (index != -1) {
    printf("%d was %s\n", target, tab.records[index].value);
  } else {
    printf("%d was NOT FOUND\n", target);
  }

  return 0;
}

// 実行結果
// TABLE: [ {4, AAA} {1, AAA} {0, AAA} {2, AAA} {3, AAA} ]
// LENGTH: 5
//
// Type in a key (>= 0) and a field. (example: "100 BBB")
// 100 B
// TABLE: [ {4, AAA} {1, AAA} {0, AAA} {2, AAA} {3, AAA} {100, B} ]
// LENGTH: 6
//
// 3 was AAA
// TABLE: [ {4, AAA} {1, AAA} {0, AAA} {2, AAA} {100, B} ]
// LENGTH: 5
//
// 3 was NOT FOUND.
