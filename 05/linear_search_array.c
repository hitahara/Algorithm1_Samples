#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 文字列化.
 */
#define STRINGFY(s) #s
/**
 * @brief 定義された値に変化しSTRINGFY.
 */
#define DEF_STRINGFY(def) STRINGFY(def)

#define MAX_RECORDS 1000
#define MAX_VALUE_SIZE 32
#define SAMPLE_RECORDS_SIZE 10

typedef struct record {
  int key;
  char value[MAX_VALUE_SIZE];
} record;

typedef struct table {
  int length;
  record records[MAX_RECORDS];
} table;

void fisher_yates_shuffle(int*, int);

void insert_tail(table*, record*);
void erase(table*, int);

int search(table*, int);

void cli_insert_tail(table*);
void print_table(table*);

/**
 * @brief 配列を fisher-yates shuffle.
 */
void fisher_yates_shuffle(int* array, int length) {
  int i = length;
  while (i > 1) {
    int j = rand() % i;
    i--;
    int t = array[i];
    array[i] = array[j];
    array[j] = t;
  }
}

/**
 * @brief tableのrecordsにtargetが存在するか調べ,存在する場合該当するキー,存在しない場合-1==2^64-1を返す.
 */
int search(table* tab, int target) {
  tab->records[tab->length].key = target;
  int searching_pos = 0;
  while (target != tab->records[searching_pos].key) {
    searching_pos++;
  }
  return searching_pos < tab->length ? searching_pos : -1;
}

/**
 * @brief tableのrecordsの末尾にデータを挿入する.
 */
void insert_tail(table* tab, record* rec) {
  if (tab->length >= MAX_RECORDS - 1) {
    printf("ERROR: No more record can be inserted into table.\n");
    return;
  }

  tab->records[tab->length] = *rec;
  tab->length++;
}

/**
 * @brief tableのrecordsの位置posを削除する.
 */
void erase(table* tab, int pos) {
  for (int i = pos; i < tab->length - 1; i++) {
    tab->records[i] = tab->records[i + 1];
  }
  tab->length--;
}

/**
 * @brief cliからrecordの内容を読み取り,tableの末尾に挿入.
 */
void cli_insert_tail(table* tab) {
  printf("Type in a key (>= 0) and a field. (example: \"100 BBB\")\n");
  while (true) {
    record rec;
    scanf("%d %s", &rec.key, rec.value);
    if (search(tab, rec.key) != -1) {
      printf("The key is already used.\n");
    } else {
      insert_tail(tab, &rec);
      return;
    }
  }
}

void print_table(table* tab) {
  printf("TABLE: [ ");
  for (int i = 0; i < tab->length; i++) {
    printf("{%d, %s} ", tab->records[i].key, tab->records[i].value);
  }
  printf("]\n");
  printf("LENGTH: %d\n\n", tab->length);
}

int main() {
  int keys[SAMPLE_RECORDS_SIZE];
  for (int i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    keys[i] = i;
  }
  fisher_yates_shuffle(keys, SAMPLE_RECORDS_SIZE);

  table tab = {0};

  // add default records
  for (int i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    record rec = {keys[i], "AAA"};
    insert_tail(&tab, &rec);
  }
  print_table(&tab);

  cli_insert_tail(&tab);
  print_table(&tab);

  bool found = search(&tab, 5) != -1;
  printf("5 was %s\n", found ? "FOUND." : "NOT FOUND.");

  erase(&tab, search(&tab, 5));
  print_table(&tab);

  found = search(&tab, 5) != -1;
  printf("5 was %s\n", found ? "FOUND." : "NOT FOUND.");

  return 0;
}
