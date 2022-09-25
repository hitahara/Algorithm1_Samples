#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fisher_yates_shuffle(int* array, int array_size);

typedef struct record record;
typedef struct table table;  // array用のtable,list用と異なる.

record* init_record(int key, const char* field);

void insert_tail(table* tab, record* rec);
void erase(table* tab, int pos);

bool search_existence(table* tab, int target);
int search_index(table* tab, int target);

record* cli_record();
void cli_insert_tail(table* tab);
void print_record(record* rec);
void print_table(table* tab);
void print_search_existence(table* tab, int target);

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
 * @brief 文字列化.
 */
#define STRINGFY(s) #s
/**
 * @brief 定義された値に変化しSTRINGFY.
 */
#define DEF_STRINGFY(def) STRINGFY(def)

#define MAX_RECORDS 1000
#define MAX_FIELD_MEMORY 32
#define SAMPLE_RECORDS_SIZE 10

typedef struct record {
  int key;
  char field[MAX_FIELD_MEMORY];
} record;

typedef struct table {
  int length;
  record records[MAX_RECORDS];
} table;

/**
 * @brief recordの初期化.
 */
record* init_record(int key, const char* field) {
  if (strlen(field) > MAX_FIELD_MEMORY + 1) {
    fprintf(stderr, "ERROR: \"field\" is too large.\n");
    exit(1);
  }

  record* rec = (record*)malloc(sizeof(record));
  rec->key = key;
  strncpy(rec->field, field, MAX_FIELD_MEMORY);

  return rec;
}

/**
 * @brief tableのrecordsにtargetが存在するか調べる.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 * @return targetがrecords内に存在するかを示すbool.
 */
bool search_existence(table* tab, int target) {
  tab->records[tab->length].key = target;
  int searching_pos = 0;
  while (target != tab->records[searching_pos].key) {
    searching_pos++;
  }
  return searching_pos < tab->length;
}

/**
 * @brief tableのrecordsにtargetが存在するか調べ,存在する場合該当するキー,存在しない場合-1==2^64-1を返す.
 */
int search_index(table* tab, int target) {
  tab->records[tab->length].key = target;
  int searching_pos = 0;
  while (target != tab->records[searching_pos].key) {
    searching_pos++;
  }
  return searching_pos < tab->length ? searching_pos : -1;
}

/**
 * @brief tableのrecordsの末尾にデータを挿入する.
 * @param[in] tab 挿入対象のrecordsをもつtableのポインタ.
 * @param[in] rec 挿入するrecord.
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
 * @param[in] tab 削除対象のrecordsをもつtableのポインタ.
 * @param[in] pos 削除するrecordsの位置.
 */
void erase(table* tab, int pos) {
  for (int i = pos; i < tab->length - 1; i++) {
    tab->records[i] = tab->records[i + 1];
  }
  tab->length--;
}

record* cli_record() {
  printf("Type in a key (>= 0) and a field. (example: \"100 BBB\")\n");
  record* rec = (record*)malloc(sizeof(record));
  scanf("%d %s", &rec->key, rec->field);
  return rec;
}

/**
 * @brief cliからrecordの内容を読み取り,tableの末尾に挿入.
 */
void cli_insert_tail(table* tab) {
  printf("Enter a record that will be inserted at the tail.\n");
  while (true) {
    record* rec = cli_record();
    if (search_existence(tab, rec->key)) {
      printf("The key is already used.\n");
    } else {
      insert_tail(tab, rec);
      return;
    }
  }
}

void print_record(record* rec) {
  printf("%d, %s\n", rec->key, rec->field);
}

void print_table(table* tab) {
  printf("================================\n");
  printf("TABLE: [\n");
  for (int i = 0; i < tab->length; i++) {
    print_record(&tab->records[i]);
  }
  printf("]\nLENGTH: %d\n", tab->length);
  printf("================================\n");
}

/**
 * @brief search_existence確認用プリント関数.
 */
void print_search_existence(table* tab, int target) {
  bool b = search_existence(tab, target);
  printf("\"%d\" was %s\n", target, b ? "FOUND." : "NOT FOUND.");
}

int main() {
  int keys[SAMPLE_RECORDS_SIZE];
  for (int i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    keys[i] = i;
  }
  fisher_yates_shuffle(keys, sizeof(keys) / sizeof(int));

  table tab = {.length = 0};

  for (int i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    record* rec = init_record(keys[i], "AAA");
    insert_tail(&tab, rec);
  }

  cli_insert_tail(&tab);

  print_table(&tab);
  print_search_existence(&tab, 5);

  erase(&tab, search_index(&tab, 5));

  print_table(&tab);
  print_search_existence(&tab, 5);

  return 0;
}
