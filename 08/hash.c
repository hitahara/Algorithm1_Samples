#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record record;
typedef struct hash_record hash_record;
typedef struct hash hash;

record *init_record(int key, const char *field);
hash_record *init_hash_record(record *rec);
hash *init_hash();
void release_hash(hash *h);

int hash_func(int i, int max_size);
void insert(hash *has, record *rec);
void search_existence_and_record(hash *has, int target, bool *found, record **target_rec);

void print_record(record *rec);
void print_hash(hash *h);
void print_search_existence(hash *has, int target);

record *cli_record();

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
#define MAX_ARRAY_MEMORY 4096
/**
 * @brief レコードのフィールドのデータが占有できる最大バイト数.
 */
#define MAX_FIELD_MEMORY 32
/**
 * @brief サンプルで使用するrecordsの量
 */
#define INITIAL_NUM_RECORDS 10

/**
 * @brief レコード：keyとfieldをもった構造体
 */
struct record {
  int key;                      /** int型のキー. key==-1or2^64-1を例外処理に用いている*/
  char field[MAX_FIELD_MEMORY]; /** データを保持するchar型の配列. */
};

/**
 * @brief hash_recordでのデータが存在するかなどの状態.
 */
typedef enum status {
  FREE,
  USED,
  DELETED,
} status;

/**
 * @brief hash用レコード.
 */
struct hash_record {
  record *data;
  status mark; /** statusの一番上FREEがデフォルトで. */
};

/**
 * @brief ハッシュの開番地法,線形走査で.
 */
struct hash {
  int size; /** 配列がとれる最大長. */
  int filled_size;
  hash_record table[MAX_ARRAY_MEMORY / sizeof(hash_record)]; /** record型で表された配列. */
};

/**
 * @brief recordの初期化.
 * @param[in] key 指定するキー.
 * @param[in] field const char*,動的にするのであればchar*,サイズはMAX_FIELD_MEMORYで定義.
 * @return 初期化されたrecordのポインタ.
 */
record *init_record(int key, const char *field) {
  if (strlen(field) > MAX_FIELD_MEMORY + 1) {
    fprintf(stderr, "ERROR: \"field\" is too large.\n");
    exit(1);
  }

  record *rec = (record *)malloc(sizeof(record));
  rec->key = key;
  strncpy(rec->field, field, MAX_FIELD_MEMORY);

  return rec;
}

hash_record *init_hash_record(record *rec) {
  hash_record *hr = (hash_record *)malloc(sizeof(hash_record));
  hr->data = rec;
  hr->mark = USED;
  return hr;
}

/**
 * @brief tableの初期化.
 * @return 初期化されたtableのポインタ.
 */
hash *init_hash() {
  hash *h = (hash *)malloc(sizeof(hash));
  h->size = MAX_ARRAY_MEMORY / sizeof(hash_record);
  h->filled_size = 0;

  return h;
}

/**
 * @brief hashのメモリを解放.
 * @param[in] h メモリを開放するhash.
 */
void release_hash(hash *h) {
  for (int i = 0; i < h->size; i++) {
    if (h->table[i].mark == FREE) {
      continue;
    }
    free(h->table[i].data);
  }
  free(h);
  h = NULL;
}

/**
 * @brief 雑なハッシュ関数.
 * @param[in] i 変換するインデックス.
 * @param[in] max_size hashがとれる最大のインデックス+1.
 */
int hash_func(int i, int max_size) {
  double m = i;
  m = (3.14 / i) * 10000;
  // printf("%lf\n", m);
  i = (int)m % max_size;
  // printf("%d\n", i);

  return (int)i;
}

/**
 * @brief hashにrecordを挿入する.
 * @param[in] has recordを挿入するhashのポインタ.
 * @param[in] rec 挿入するrecordのポインタ.
 */
void insert(hash *has, record *rec) {
  if (has->filled_size >= has->size - 1) {
    fprintf(stderr, "ERROR: The hash table is full.\n");
    return;
  } else {
    int h = hash_func(rec->key, has->size);
    while (has->table[h].mark == USED) {
      if (rec->key == has->table[h].data->key) {
        fprintf(stderr, "ERROR: The key already exists in the hash table.\n");
        exit(1);
      } else {
        h = (h + 1) % has->size;
      }
    }
    has->table[h].mark = USED;
    has->table[h].data = rec;
    has->filled_size++;
  }
}

/**
 * @brief hashのtableにtargetが存在するか調べ,recordを取り出す.
 * @param[in] has targetが存在するか調べるhash.
 * @param[in] target 調べるキー.
 * @param[out] found targetがhash内に存在するかの真理値を返す.
 * @param[out] target_rec target==target_rec->keyのrecordを返す.
 */
void search_existence_and_record(hash *has, int target, bool *found, record **target_rec) {
  int pos = hash_func(target, has->size);
  int loop = 0;
  while (has->table[pos].mark == USED && target != has->table[pos].data->key) {
    pos = (pos + 1) % has->size;
    loop++;
    if (loop <= has->size) {
      *found = false;
      *target_rec = NULL;
      return;
    }
  }
  *found = has->table[pos].mark == USED;
  *target_rec = *found ? has->table[pos].data : NULL;
}

/**
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record *rec) {
  printf("[%06zu, %s]\n", rec->key, rec->field);
}

/**
 * @brief table確認用プリント関数.
 * @param[in] tab プリントする配列をもつtableのポインタ.
 */
void print_hash(hash *h) {
  printf("================================\n");
  printf("TABLE: [\n");
  for (int i = 0; i < h->size; i++) {
    if (h->table[i].mark == FREE) {
      printf("Index %d is FREE.\n", i);
      continue;
    }
    print_record(h->table[i].data);
  }
  printf("]\nHASH SIZE: %d\n", h->size);
  printf("================================\n");
}

/**
 * @brief search_existence_and_record確認用プリント関数.
 * @param[in] has targetが存在するか調べるhash.
 * @param[in] target 調べるキー.
 */
void print_search_existence(hash *has, int target) {
  bool found;
  record *target_rec;
  search_existence_and_record(has, target, &found, &target_rec);
  printf("\"%d\" was %s.\n", target, found ? "FOUND" : "NOT FOUND");
  if (found) {
    printf("Hash table index is %d.\n", hash_func(target, has->size));
    printf("Found data is...\n");
    print_record(target_rec);
  }
}

/**
 * @brief cliからrecordの内容を読み取りrecordに格納.
 * @return cliから読み取った内容で生成したrecordのポインタ
 */
record *cli_record() {
  record *rec;
  int key = -1;
  char field[MAX_FIELD_MEMORY];

  printf("Type in a key >= 0 and a field. (example: \"10001 BBB\")\n");
  printf(STRINGFY(MAX_FIELD_MEMORY) "=" DEF_STRINGFY(MAX_FIELD_MEMORY) "\n");

  while (key == (long unsigned int)-1) {
    scanf("%d", &key);
  }
  scanf("%" DEF_STRINGFY(MAX_FIELD_MEMORY) "s%*[^\n]", field);

  rec = init_record(key, field);

  return rec;
}

int main() {
  hash *has = init_hash();
  for (int i = 0; i < 100; i++) {
    insert(has, init_record(i, "AAAA"));
  }

  print_hash(has);
  print_search_existence(has, 30);

  release_hash(has);
  return 0;
}