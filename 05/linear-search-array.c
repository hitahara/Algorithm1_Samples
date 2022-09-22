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
 * @brief 配列がとれる最大バイト数.
 */
#define MAX_ARRAY_MEMORY 65535
/**
 * @brief レコードのフィールドのデータが占有できる最大バイト数.
 */
#define MAX_FIELD_MEMORY 32
/**
 * @brief サンプルで使用するrecordsの量
 */
#define SAMPLE_RECORDS_SIZE 10

/**
 * @brief レコード：keyとfieldをもった構造体
 */
typedef struct record {
  size_t key;                   /** size_t型のキー. key==-1or2^64-1を例外処理に用いている*/
  char field[MAX_FIELD_MEMORY]; /** データを保持するchar型の配列. */
} record;

/**
 * @brief 表：recordの配列とそのメタデータを保持する構造体.
 */
typedef struct table {
  size_t records_length;                             /** 配列の長さ. */
  size_t size;                                       /** 配列がとれる最大長. */
  record records[MAX_ARRAY_MEMORY / sizeof(record)]; /** record型で表された配列. */
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
 * @brief tableの初期化.
 * @return 初期化されたtableのポインタ.
 */
table* init_table() {
  table* tab = (table*)malloc(sizeof(table));
  tab->records_length = 0;
  tab->size = MAX_ARRAY_MEMORY / sizeof(record);
  return tab;
}

/**
 * @brief tableのメモリを解放.
 * @param[in] tab メモリを開放するtable.
 */
void release_table(table* tab) {
  free(tab);
  tab = NULL;
}

/**
 * @brief tableのrecordsにtargetが存在するか調べる.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 * @return targetがrecords内に存在するかを示すbool.
 */
bool search_existence(table* tab, size_t target) {
  tab->records[tab->records_length].key = target;
  size_t searching_pos = 0;
  // while(searching_pos < tab->records_length && target != tab->records[searching_pos].key)
  while (target != tab->records[searching_pos].key) {
    searching_pos++;
  }
  return searching_pos < tab->records_length;
}

/**
 * @brief tableのrecordsにtargetが存在するか調べ,存在する場合該当するキー,存在しない場合-1==2^64-1を返す.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 * @return targetを指すキーか,無い場合は-1==2^64-1,恐らくシステムが32bitか64bitで変化(あくまで予想).
 */
size_t search_index(table* tab, size_t target) {
  tab->records[tab->records_length].key = target;
  size_t searching_pos = 0;
  while (target != tab->records[searching_pos].key) {
    searching_pos++;
  }
  return searching_pos < tab->records_length ? searching_pos : -1;
}

/**
 * @brief tableのrecordsの末尾にデータを挿入する.
 * @param[in] tab 挿入対象のrecordsをもつtableのポインタ.
 * @param[in] rec 挿入するrecord.
 */
void insert_tail(table* tab, record* rec) {
  if (tab->records_length >= tab->size - 1) {
    printf("ERROR: No more record can be inserted into table.\n");
    return;
  }

  // 0オリジン
  tab->records[tab->records_length] = *rec;
  tab->records_length++;
}

/**
 * @brief tableのrecordsの位置posを削除する.
 * @param[in] tab 削除対象のrecordsをもつtableのポインタ.
 * @param[in] pos 削除するrecordsの位置.
 */
void delete_record(table* tab, size_t pos) {
  for (size_t i = pos; i < tab->records_length - 1; i++) {
    tab->records[i] = tab->records[i + 1];
  }
  tab->records_length--;
}

/**
 * @brief cliからrecordの内容を読み取りrecordに格納.
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
 * @brief cliからrecordの内容を読み取り,tableの末尾に挿入.
 * @param[in] tab 挿入対象のrecordsをもつtableのポインタ.
 */
void cli_insert_tail(table* tab) {
  printf("ENETER A RECORD THAT WILL BE INSERTED AT THE TAIL.\n");
  record* scanned_rec = NULL;
  while (true) {
    scanned_rec = cli_record();
    if (search_existence(tab, scanned_rec->key)) {
      printf("The key is already used.\n");
    } else {
      break;
    }
  }
  insert_tail(tab, scanned_rec);
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
 * @param[in] tab プリントする配列をもつtableのポインタ.
 */
void print_table(table* tab) {
  printf("================================\n");
  printf("TABLE: [\n");
  for (size_t i = 0; i < tab->records_length; i++) {
    print_record(&tab->records[i]);
  }
  printf("]\nTABLE LENGTH: %ld\n", tab->records_length);
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
    insert_tail(tab, rec);
  }

  cli_insert_tail(tab);

  print_table(tab);
  print_search_existence(tab, 5);

  delete_record(tab, search_index(tab, 5));

  print_table(tab);
  print_search_existence(tab, 5);

  release_table(tab);
  return 0;
}