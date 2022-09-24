#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#define MAX_ARRAY_MEMORY MAX_RECORDS_SIZE * sizeof(record *)
/**
 * @brief レコードの最大長.
 */
#define MAX_RECORDS_SIZE 10000000
#define CHUNK_SIZE 100000

/**
 * @brief レコードのフィールドが保持できるcharの個数(\0を含まない).
 */
#define MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER 32
/**
 * @brief レコードのフィールドが実際に保持できるcharの個数(\0を含む).
 */
#define MAX_FIELD_SIZE MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER + 1

typedef struct record record;
typedef struct sequence sequence;

record *init_record(int key, char *field);
sequence *init_sequence();
void release_record(record **rec);
void release_sequence(sequence **seq);

bool is_record_a_larger_than_b(record *a, record *b);
bool is_a_larger_than_b_records(record **records, int a, int b);
bool is_a_larger_than_b_sequence(sequence *seq, int a, int b);

void insert_tail(sequence *seq, record *rec);

void binary_search_existence_and_index(sequence *seq, int target, bool *found, int *index);

void quick_sort(sequence *seq);
void q_sort_refined(sequence *seq, int left, int right);

void print_record(record *rec);
void print_sequence(sequence *seq);

void run_sort_algorithm(sequence *seq, void (*sort_algorithm)(sequence *));

void fisher_yates_shuffle(int *array, int array_size);
void check_seq_sorted(sequence *seq);

/**
 * @brief レコード：keyとfieldをもった構造体
 */
struct record {
  int key;                    /** int型のキー. key==-1or2^64-1を例外処理に用いている*/
  char field[MAX_FIELD_SIZE]; /** データを保持するchar型の配列. */
};

/**
 * @brief 配列とそのメタデータを保持する構造体.
 */
struct sequence {
  int elements_length; /** 配列の長さ. */
  int size;            /** 配列がとれる最大長. */
  int elements_chunk_size;
  record ***elements_chunk; /** recordのポインタの配列. */
};

/**
 * @brief recordの初期化.
 * @param[in] key 初期化時に指定するキー.
 * @param[in] field 静的ならばconst char*,動的ならばchar*にしなければいけない,
 * サイズはMAX_FIELD_SIZEで定義.初期化時に指定するフィールド.
 * @return 初期化されたrecordのポインタ.
 */
record *init_record(int key, char *field) {
  if (strlen(field) > MAX_FIELD_SIZE) {
    fprintf(stderr, "ERROR: \"field\" is too large.\n");
    exit(1);
  }
  record *rec = (record *)malloc(sizeof(record));
  if (rec == NULL) {
    fprintf(stderr, "ERROR: Not enough memory for new record.\n");
    exit(1);
  }

  rec->key = key;
  strncpy(rec->field, field, MAX_FIELD_SIZE);

  return rec;
}
/**
 * @brief sequenceの初期化.
 * @return 初期化されたsequenceのポインタ.
 */
sequence *init_sequence() {
  sequence *s = (sequence *)malloc(sizeof(sequence));
  s->elements_length = 0;
  if (MAX_ARRAY_MEMORY >= 10000000000) {
    fprintf(stderr, "ERROR: \"MAX_ARRAY_MEMORY\" is too large.\n");
    exit(1);
  }
  s->size = MAX_RECORDS_SIZE;
  int elements_chunk_size = ((MAX_RECORDS_SIZE - 1) >= 0 ? (MAX_RECORDS_SIZE - 1) : 0) / CHUNK_SIZE + 1;
  s->elements_chunk_size = elements_chunk_size;

  s->elements_chunk = (record ***)malloc(sizeof(record **) * elements_chunk_size);
  for (int i = 0; i < elements_chunk_size; ++i) {
    s->elements_chunk[i] = (record **)malloc(sizeof(record *) * CHUNK_SIZE);
  }
  printf("elements_chunk_size=%zu\n", s->elements_chunk_size);

  return s;
}

/**
 * @brief recordのメモリを解放.
 * @param[in] rec メモリを開放するrecord.
 */
void release_record(record **rec) {
  free(*rec);
  *rec = NULL;
}

/**
 * @brief sequenceのメモリを解放.
 * @param[in] seq メモリを開放するsequence.
 */
void release_sequence(sequence **seq) {
  // seq内のmallocで生成されたrecord群のメモリも解放.
  for (int j = 0; j < (*seq)->elements_chunk_size; ++j) {
    for (int i = 0; i < CHUNK_SIZE; i++) {
      if ((*seq)->elements_chunk[j][i] != NULL) {
        release_record(&(*seq)->elements_chunk[j][i]);
      }
    }
  }

  free((*seq)->elements_chunk);
  free(*seq);
  *seq = NULL;
}

record **get_record_index(sequence *seq, int index) {
  return &(seq->elements_chunk[index / CHUNK_SIZE][index % CHUNK_SIZE]);
}

void insert_record_index(sequence *seq, int index, record *rec) {
  seq->elements_chunk[index / CHUNK_SIZE][index % CHUNK_SIZE] = rec;
}

/**
 * @brief 2つのreocrdをkeyで評価.
 * @param[in] a 評価するrecord.
 * @param[in] b 評価するrecord.
 * @return a->key > b->keyの場合:true,a->key > b->keyの場合:false.
 */
bool is_record_a_larger_than_b(record *a, record *b) {
  return a->key > b->key;
}

/**
 * @brief 2つのインデックスでのkeyを評価.
 * @param[in] records 評価するインデックスが指すrecordのポインタの配列.
 * @param[in] a 評価するインデックスのなかで最小(a<b).
 * @param[in] b 評価するインデックスのなかで最大(a<b).
 * @return a>bの場合:true,a<bの場合:false.
 */
bool is_a_larger_than_b_records(record **records, int a, int b) {
  return is_record_a_larger_than_b(records[a], records[b]);
}

/**
 * @brief 2つのインデックスでのkeyを評価.
 * @param[in] seq 評価するインデックスが指すrecordのポインタの配列をもつsequenceのポインタ.
 * @param[in] a 評価するインデックスのなかで最小(a<b).
 * @param[in] b 評価するインデックスのなかで最大(a<b).
 * @return a>bの場合:true,a<bの場合:false.
 */
bool is_a_larger_than_b_sequence(sequence *seq, int a, int b) {
  return is_record_a_larger_than_b(*get_record_index(seq, a), *get_record_index(seq, b));
}

/**
 * @brief sequenceの末尾にデータを挿入する.
 * @param[in] seq recを挿入するsequenceのポインタ.
 * @param[in] rec 挿入するrecordのポインタ.
 */
void insert_tail(sequence *seq, record *rec) {
  if (seq->elements_length >= seq->size) {
    printf("ERROR: No more record can be inserted into table.\n");
    return;
  }

  if (*get_record_index(seq, seq->elements_length) == NULL) {
    insert_record_index(seq, seq->elements_length, rec);
  } else {
    release_record(get_record_index(seq, seq->elements_length));
    printf("The inserting index was dirty, so it was cleaned.\n");

    insert_record_index(seq, seq->elements_length, rec);
  }
  seq->elements_length++;
}

/**
 * @brief クイックソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void quick_sort(sequence *seq) {
  int left = 0, right = seq->elements_length - 1;

  printf("Using version: " STRINGFY(q_sort_refined) "\n");
  q_sort_refined(seq, left, right);
}

/**
 * @brief クイックソート.
 * @param[in] seq ソートするsequenceのポインタ.
 * @param[in] left 処理を行う塊のなかで最も小さいインデックス.
 * @param[in] right 処理を行う塊のなかで最も大きいインデックス.
 */
void q_sort_refined(sequence *seq, int left, int right) {
  if (left < right) {
    int pivot_index = (left + right) / 2;
    record *pivot_record = *get_record_index(seq, pivot_index);
    int i = left, j = right;
    do {
      for (; is_record_a_larger_than_b(pivot_record, *get_record_index(seq, i)); ++i)
        ;
      for (; is_record_a_larger_than_b(*get_record_index(seq, j), pivot_record); --j)
        ;
      if (i <= j) {
        record **a = get_record_index(seq, i);
        record **b = get_record_index(seq, j);
        record *t = NULL;
        t = *a;
        *a = *b;
        *b = t;

        i++;
        j = (j - 1) % (int)-1;
      }
    } while (i <= j);

    q_sort_refined(seq, left, j);
    q_sort_refined(seq, i, right);
  }
}

/**
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record *rec) {
  if (rec == NULL) {
    printf("ERROR: Record is NULL\n");
  } else {
    printf("[%06zu, %s]\n", rec->key, rec->field);
  }
}

/**
 * @brief sequence確認用プリント関数.
 * @param[in] seq プリントするsequenceのポインタ.
 */
void print_sequence(sequence *seq) {
  if (seq == NULL) {
    printf("ERROR: Sequence is NULL\n");
  } else {
    printf("\n");
    printf("================================\n");
    printf("Printing sequence\n");
    printf("================================\n");
    for (int i = 0; i < seq->elements_length; i++) {
      print_record(*get_record_index(seq, i));
    }
    printf("================================\n");
    printf("Sequence size: (%zu/%zu)\n", seq->elements_length, seq->size);
    printf("================================\n");
    printf("\n");
  }
}

void run_sort_algorithm(sequence *seq, void (*sort_algorithm)(sequence *)) {
  double start_clock, end_clock;

  start_clock = (double)clock();
  sort_algorithm(seq);
  end_clock = (double)clock();

  printf("CPU execution time: %.8lf s.\n", (end_clock - start_clock) / CLOCKS_PER_SEC);
}

/**
 * @brief seqのキーを見て,昇順になっているかを確認しプリント.
 * @param[in] seq 整列(昇順)になっているかを確認するseq.
 */
void check_seq_sorted(sequence *seq) {
  bool sorted = true;
  int former = 0;
  for (int i = 0; i < seq->elements_length && sorted; ++i) {
    if (former > (*get_record_index(seq, i))->key) {
      sorted = false;
    } else {
      former = (*get_record_index(seq, i))->key;
    }
  }

  printf("Sequence is ... %s\n", sorted ? "in sort." : "not in sort.");
}

int keys[MAX_RECORDS_SIZE];

int main() {
  for (int i = 0; i < MAX_RECORDS_SIZE; i++) {
    keys[i] = i;
  }

  fisher_yates_shuffle(keys, sizeof(keys) / sizeof(int));

  sequence *seq = init_sequence();
  for (int i = 0; i < MAX_RECORDS_SIZE; i++) {
    insert_tail(seq, init_record(keys[i], (char *)"AAA"));
  }

  printf("Record size: %d\n", MAX_RECORDS_SIZE);
  printf("Using algorithm: " STRINGFY(quick_sort) "\n");
  run_sort_algorithm(seq, quick_sort);

  // print_sequence(seq);
  // check_seq_sorted(seq);

  // release_sequence(&seq);
  return 0;
}

/**
 * @brief 指定されたintのポインタにある配列にfiesher-yates shuffle.
 * @param[in,out] array シャフルする配列であるintのポインタ.
 * @param[in] array_size シャッフルする配列のサイズ
 */
void fisher_yates_shuffle(int *array, int array_size) {
  int i = array_size;
  while (i > 1) {
    int j = rand() % i;
    i--;
    int t = array[i];
    array[i] = array[j];
    array[j] = t;
  }
}