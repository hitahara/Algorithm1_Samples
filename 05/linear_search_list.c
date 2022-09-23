#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// size_t型: 恐らく64bitのシステムだとlong unsigned int, 32bitのシステムだとunsigned int.

/**
 * @brief 指定されたsize_tのポインタにある配列にfiesher-yates shuffle.
 * @param[in,out] array シャフルする配列であるsize_tのポインタ.
 * @param[in] array_size シャッフルする配列のサイズ
 */
void fisher_yates_shuffle(size_t* array, size_t array_size) {
  size_t i = array_size;
  while (i > 1) {
    size_t j = rand() % i;
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

/**
 * @brief レコードのフィールドのデータが占有できる最大バイト数.
 */
#define MAX_FIELD_MEMORY 32
/**
 * @brief サンプルで使用するrecordsの量.
 */
#define SAMPLE_RECORDS_SIZE 10

/**
 * @brief レコード：keyとfieldをもった構造体.
 */
typedef struct record {
  size_t key;                   /** size_t型のキー. key==-1or2^64-1を例外処理に用いている*/
  char field[MAX_FIELD_MEMORY]; /** データを保持するchar型の配列. */
} record;

/**
 * @brief データと次のcellを保持する構造体.
 */
typedef struct cell {
  record* rec;       /** record型で表されたデータ. */
  struct cell* next; /** 次のcellを指すポインタ. */
} cell;

/**
 * @brief cellのポインタの型.
 */
typedef cell* list;

/**
 * @brief  表：先頭と末尾のダミーlistを保持する構造体.
 */
typedef struct table {
  list header;
  list sentinel;
} table;

/**
 * @brief recordの初期化.
 * @param[in] key 指定するキー.
 * @param[in] field const char*,動的にするのであればchar*,サイズはMAX_FIELD_MEMORYで定義.
 * @return 初期化されたrecordのポインタ.
 */
record* init_record(size_t key, const char* field) {
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
 * @brief cellの初期化.
 * @return 初期化された,データやポインタが代入されていないcellのポインタ.
 */
cell* init_cell(record* rec) {
  cell* c = (cell*)malloc(sizeof(cell));
  c->rec = rec;
  return c;
}

/**
 * @brief listの初期化.
 * @return NULLのlist.
 */
list init_list() {
  record* rec = init_record(-1, "FAKE");
  list l = init_cell(rec);
  return l;
}

/**
 * @brief tableの初期化.
 * @return 初期化されたheaderとsentinelをもったtableのポインタ.
 */
table* init_table() {
  table* tab = (table*)malloc(sizeof(table));
  tab->sentinel = init_list();
  tab->header = init_list();
  tab->header->next = tab->sentinel;
  return tab;
}

/**
 * @brief 引数のcellを削除する.
 * @param[in] c 削除するcellのポインタ.
 */
void dispose(cell* c) {
  free(c->rec);
  c->rec = NULL;
  free(c);
  c = NULL;
}

/**
 * @brief tableのメモリ解放する.
 * @param[in] tab メモリ解放するtableのポインタ.
 */
void release_table(table* tab) {
  list disposing_cell = tab->header->next;
  list next_cell = NULL;
  for (; disposing_cell != tab->sentinel;) {
    next_cell = disposing_cell->next;
    dispose(disposing_cell);
    disposing_cell = next_cell;
  }

  dispose(tab->header);
  dispose(tab->sentinel);

  free(tab);
  tab = NULL;
}

/**
 * @brief tableのリストの先頭にlist／cellのポインタを挿入する.
 * @param[in] tab 挿入対象のリストをもつtableのポインタ.
 * @param[in] p 挿入するlist／cellのポインタ.
 */
void insert_head_list(table* tab, list p) {
  p->next = tab->header->next;
  tab->header->next = p;
}
/**
 * @brief tableのリストの先頭にデータを挿入する.
 * @param[in] tab 挿入対象のリストをもつtableのポインタ.
 * @param[in] rec 挿入するrecord.
 */
void insert_head_record(table* tab, record* rec) {
  list p = init_cell(rec);
  insert_head_list(tab, p);
}

/**
 * @brief tableのリストにtargetが存在するか調べる.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 * @return targetがrecords内に存在するかを示すbool.
 */
bool search_existence(table* tab, size_t target) {
  tab->sentinel->rec->key = target;
  list previous = tab->header;
  list p = tab->header->next;
  while (target != p->rec->key) {
    previous = p;
    p = p->next;
  }
  bool found = p != tab->sentinel;
  if (found) {
    previous->next = p->next;
    insert_head_list(tab, p);
  }
  return found;
}

/**
 * @brief tableのリストにtargetを探し，存在する場合その前のlist／cellのポインタを返す.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 * @return targetを示すlistを指すlistを返す.
 */
list search_previous(table* tab, size_t target) {
  tab->sentinel->rec->key = target;
  list previous = tab->header;
  list p = tab->header->next;
  while (target != p->rec->key) {
    previous = p;
    p = p->next;
  }
  bool found = p != tab->sentinel;
  return found ? previous : NULL;
}

/**
 * @brief tableからpreviousの次のlist／cellのポインタを削除
 * @param[in] previous previous->next==削除するlist／cellのポインタ
 */
void delete_next(table* tab, list previous) {
  list p = previous->next;
  previous->next = p->next;
  dispose(p);
}

/**
 * @brief tableのリストにtargetを探し，存在する場合はリストから削除.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 削除するキー.
 */
void delete_target(table* tab, size_t target) {
  list prev_l = search_previous(tab, target);
  delete_next(tab, prev_l);
}

/**
 * @brief cliからrecordの内容をsscanf_s.
 * @return cliから読み取った内容で生成したrecordのポインタ
 */
record* cli_record() {
  record* rec;
  size_t key = -1;
  char field[MAX_FIELD_MEMORY];

  printf("Type in a key >= 0 and a field. (example: \"10001 BBB\")\n");
  printf(STRINGFY(MAX_FIELD_MEMORY) "=" DEF_STRINGFY(MAX_FIELD_MEMORY) "\n");

  while (key == (long unsigned int)-1) {
    scanf("%zu", &key);
  }
  scanf("%" DEF_STRINGFY(MAX_FIELD_MEMORY) "s%*[^\n]", field);

  rec = init_record(key, field);

  return rec;
}

/**
 * @brief cliからrecordの内容を読み取り,tableの先頭に挿入.
 * @param[in] tab 挿入対象のrecordsをもつtableのポインタ.
 */
void cli_insert_head(table* tab) {
  printf("ENETER A RECORD THAT WILL BE INSERTED AT THE HEAD.\n");
  record* scanned_rec = NULL;
  while (true) {
    scanned_rec = cli_record();
    if (search_existence(tab, scanned_rec->key)) {
      printf("The key is already used.\n");
    } else {
      break;
    }
  }
  insert_head_record(tab, scanned_rec);
}

/**
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record* rec) {
  printf("%08zu, %s\n", rec->key, rec->field);
}

/**
 * @brief table確認用プリント関数.
 * @param[in] tab プリントするリストをもつtableのポインタ.
 */
void print_table(table* tab) {
  size_t record_count = 0;
  printf("================================\n");
  printf("TABLE: [\n");
  for (list c = tab->header->next; c != tab->sentinel; c = c->next, record_count++) {
    print_record(c->rec);
  }
  printf("]\nTABLE LENGTH: %ld\n", record_count);
  printf("================================\n");
}

/**
 * @brief search_existence確認用プリント関数.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 */
void print_search_existence(table* tab, size_t target) {
  bool b = search_existence(tab, target);
  printf("\"%zu\" was %s\n", target, b ? "FOUND." : "NOT FOUND.");
}

int main() {
  size_t keys[SAMPLE_RECORDS_SIZE];
  for (size_t i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    keys[i] = i;
  }
  fisher_yates_shuffle(keys, sizeof(keys) / sizeof(size_t));

  table* tab = init_table();
  record* rec = NULL;
  for (size_t i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    rec = init_record(keys[i], "AAA");
    insert_head_record(tab, rec);
  }

  cli_insert_head(tab);

  print_table(tab);
  print_search_existence(tab, 5);
  print_table(tab);

  delete_target(tab, 5);

  print_table(tab);
  print_search_existence(tab, 5);

  release_table(tab);
  return 0;
}