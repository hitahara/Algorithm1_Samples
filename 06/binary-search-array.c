#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// size_t型: 恐らく64bitのシステムだとlong unsigned int, 32bitのシステムだとunsigned int.

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
record *init_record(size_t key, const char *field) {
  if (strlen(field) > MAX_FIELD_MEMORY + 1) {
    fprintf(stderr, "ERROR: \"field\" is too large.\n");
    exit(1);
  }

  record *rec = (record *)malloc(sizeof(record));
  rec->key = key;
  strncpy(rec->field, field, MAX_FIELD_MEMORY);

  return rec;
}

/**
 * @brief tableの初期化.
 * @return 初期化されたtableのポインタ.
 */
table *init_table() {
  table *tab = (table *)malloc(sizeof(table));
  tab->records_length = 0;
  tab->size = MAX_ARRAY_MEMORY / sizeof(record);
  return tab;
}

/**
 * @brief tableのメモリを解放.
 * @param[in] tab メモリを開放するtable.
 */
void release_table(table *tab) {
  free(tab);
  tab = NULL;
}

/**
 * @brief tableのrecordsにtargetが存在するか調べ,その時による真理値とインデックスを返す.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 * @param[out] found targetがtab内に存在するかの真理値.
 * @param[out] index targetが存在する場合キーのインデックスを指す,無い場合は-1==2^64-1.
 */
void linear_search_existence_and_index(table *tab, size_t target, bool *found, size_t *index) {
  tab->records[tab->records_length].key = target;
  size_t searching_pos = 0;
  while (target != tab->records[searching_pos].key) {
    searching_pos++;
  }
  *found = searching_pos < tab->records_length;
  *index = *found ? searching_pos : -1;
}

/**
 * @brief 二分探索でtableのrecordsにtargetが存在するか調べ,その時による真理値とインデックスを返す.
 * @param[in] tab targetが存在するか調べるtable,整列になっている.
 * @param[in] target 調べるキー.
 * @param[out] found targetがtab内に存在するかの真理値.
 * @param[out] index targetが存在する場合キーのインデックスを指す,無い場合はtarget以上の最小キー.
 */
void binary_search_existence_and_index(table *tab, size_t target, bool *found, size_t *index) {
  long int lo = 0, hi = tab->records_length - 1, mid;
  while (lo <= hi) {
    // whileから出るワンステップ前になりえる状態はlo==hiまたはlo==hi+1(lo==mid+1で前者になる).
    // target <= tab->records[mid].keyによって,whileから抜けた後,
    // hiはtarget未満の最大を指すインデックス.
    // loはtarget以上の最小を指すインデックス.
    mid = (lo + hi) / 2;
    if (target <= tab->records[mid].key) {
      hi = mid - 1;
    } else {
      lo = mid + 1;
    }
  }
  //削除していない数字が合った場合をケア
  *found = lo == (long int)tab->records_length ? false : target == tab->records[lo].key;
  *index = lo;

  //二分探索アルゴリズムを一般化 〜 めぐる式二分探索法のススメ 〜
  // https://qiita.com/drken/items/97e37dd6143e33a64c8c
  // 上記と異なり
  // okは常に条件を満たすインデックス
  // ngは常に条件を満たさないインデックス
  /*
  size_t ng = -1;
  size_t ok = tab->records_length; // tab->records[tab->records_length - 1].key < targetのことを考えて
  size_t mid;
  while (ok - ng > 1)
  {
    mid = (ok + ng) / 2;
    if (target <= tab->records[mid].key)
    {
      ok = mid;
    }
    else
    {
      ng = mid;
    }
  }

  *found = ok == tab->records_length ? false : target == tab->records[ok].key;
  *index = ok;
  */
}

/**
 * @brief tableのrecordsの末尾にデータを挿入する.
 * @param[in] tab 挿入対象のrecordsをもつtableのポインタ.
 * @param[in] rec 挿入するrecord.
 */
void insert_record(table *tab, record *rec) {
  if (tab->records_length >= tab->size - 1) {
    printf("ERROR: No more record can be inserted into table.\n");
    return;
  }

  bool dummy;
  size_t inserting_pos;
  binary_search_existence_and_index(tab, rec->key, &dummy, &inserting_pos);

  for (size_t i = tab->records_length; i > inserting_pos; i--) {
    tab->records[i] = tab->records[i - 1];
  }

  tab->records[inserting_pos] = *rec;
  tab->records_length++;
}

/**
 * @brief tableのrecordsの位置posを削除する.
 * @param[in] tab 削除対象のrecordsをもつtableのポインタ.
 * @param[in] pos 削除するrecordsの位置.
 */
void delete_record(table *tab, size_t pos) {
  if (pos == (size_t)-1) {
    printf("Valid pos was not set to delte. pos=\"%zu\".\n", pos);
    return;
  }
  for (size_t i = pos; i < tab->records_length - 1; i++) {
    tab->records[i] = tab->records[i + 1];
  }
  tab->records_length--;
}

/**
 * @brief cliからrecordの内容を読み取りrecordに格納.
 * @return cliから読み取った内容で生成したrecordのポインタ
 */
record *cli_record() {
  record *rec;
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
void cli_insert(table *tab) {
  printf("ENETER A RECORD THAT WILL BE INSERTED.\n");
  record *scanned_rec = NULL;
  bool does_scanned_rec_exist = false;
  while (true) {
    scanned_rec = cli_record();
    size_t dummy;
    binary_search_existence_and_index(tab, scanned_rec->key, &does_scanned_rec_exist, &dummy);
    if (does_scanned_rec_exist) {
      printf("The key is already used.\n");
    } else {
      break;
    }
  }
  insert_record(tab, scanned_rec);
}

/**
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record *rec) {
  printf("%08zu, \"%s\"\n", rec->key, rec->field);
}

/**
 * @brief table確認用プリント関数.
 * @param[in] tab プリントする配列をもつtableのポインタ.
 */
void print_table(table *tab) {
  printf("================================\n");
  printf("TABLE: [\n");
  for (size_t i = 0; i < tab->records_length; i++) {
    print_record(&tab->records[i]);
  }
  printf("]\nTABLE LENGTH: %ld\n", tab->records_length);
  printf("================================\n");
}

/**
 * @brief binary_search_existence_and_index確認用プリント関数.
 * @param[in] tab targetが存在するか調べるtable.
 * @param[in] target 調べるキー.
 */
void print_search_existence(table *tab, size_t target) {
  bool b = false;
  size_t dummy;
  binary_search_existence_and_index(tab, target, &b, &dummy);
  printf("\"%zu\" was %s\n", target, b ? "FOUND." : "NOT FOUND.");
}

int main() {
  table *tab = init_table();
  record *rec = NULL;
  for (size_t i = 0; i < SAMPLE_RECORDS_SIZE; i++) {
    rec = init_record(i, "AAA");
    insert_record(tab, rec);
  }
  print_table(tab);
  print_search_existence(tab, 5);

  // cli_insert(tab);
  // print_table(tab);

  bool dummy;
  size_t target_pos;
  binary_search_existence_and_index(tab, 5, &dummy, &target_pos);
  delete_record(tab, target_pos);

  print_table(tab);
  print_search_existence(tab, 5);

  insert_record(tab, init_record(5, "TEST"));
  print_table(tab);

  release_table(tab);
  return 0;
}

/*
void insert_tail(table *tab, record *rec)
{
  if (tab->records_length >= tab->size - 1)
  {
    printf("ERROR: No more record can be inserted into table.\n");
    return;
  }

  tab->records[tab->records_length] = *rec;
  tab->records_length++;
  //下に整列させるアルゴリズムがなければ二分探索の前提が崩れる.
}
*/