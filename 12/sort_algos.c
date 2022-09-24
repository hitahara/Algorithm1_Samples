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
#define MAX_RECORDS_SIZE 100000  // 1037499

/**
 * @brief レコードのフィールドが保持できるcharの個数(\0を含まない).
 */
#define MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER 32
/**
 * @brief レコードのフィールドが実際に保持できるcharの個数(\0を含む).
 */
#define MAX_FIELD_SIZE MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER + 1

/**
 * @brief 交換用マクロ.typeで型指定
 */
#define SWAP(type, a, b) \
  {                      \
    type swap_tmp = a;   \
    a = b;               \
    b = swap_tmp;        \
  }

#define MIN(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
  })

#define MAX(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
  })

/** @brief スタックの配列がとれる最大バイト数. */
#define MAX_STACK_SIZE 40

typedef struct record record;
typedef struct sequence sequence;
typedef struct stack_data stack_data;
typedef struct stack stack;

record *init_record(int key, char *field);
sequence *init_sequence();
void release_record(record **rec);
void release_sequence(sequence **seq);
stack *init_stack();
void release_stack(stack **stack);
void push_stack(stack *stack, int left, int right);
void pop_stack(stack *stack, int *left, int *right);

bool is_record_a_larger_than_b(record *a, record *b);
bool is_a_larger_than_b_records(record **records, int a, int b);
bool is_a_larger_than_b_sequence(sequence *seq, int a, int b);
void compare_and_swap_records(record **records, int a, int b);
void compare_and_swap_sequence(sequence *seq, int a, int b);

void insert_tail(sequence *seq, record *rec);

void binary_search_existence_and_index(sequence *seq, int target, bool *found, int *index);

void simple_sort(sequence *seq);
void bubble_sort(sequence *seq);
void selection_sort(sequence *seq);
void insertion_sort(sequence *seq);
void shell_sort(sequence *seq);
void quick_sort(sequence *seq);
void q_sort_simple(sequence *seq, int left, int right);
void q_sort_refined(sequence *seq, int left, int right);
void q_sort_definitive(sequence *seq, int left, int right);
void heap(sequence *seq);
void down_heap(sequence *seq, int target_node_index, int end_node_index);
void merge_sort(sequence *seq);
void merge_seqs(int segment_size, sequence *from, sequence *into);
void put(sequence *from, sequence *into, int *from_inputting_index, int *into_inputted_index);
void count_sort(sequence *seq, int m);
void bin_sort(sequence *seq, int m);
void radix_sort(sequence *seq, int radix_size);
int find_max_value(sequence *seq);

void print_record(record *rec);
void print_sequence(sequence *seq);
void print_search_existence_and_index(sequence *seq, int target);

record *cli_record();
void cli_insert(sequence *seq);
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
  int elements_length;                /** 配列の長さ. */
  int size;                           /** 配列がとれる最大長. */
  record *elements[MAX_RECORDS_SIZE]; /** recordのポインタの配列. */
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
  if (MAX_ARRAY_MEMORY >= 8300000) {
    fprintf(stderr, "ERROR: \"MAX_ARRAY_MEMORY\" is too large.\n");
    exit(1);
  }
  s->size = MAX_RECORDS_SIZE;
  /*初期化時にすべてのインデックスに対してNULLになっているはず.
  for (int i = 0; i < s->size; i++)
  {
    s->elements[i] = NULL;
  }
  */
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
  for (int i = 0; i < (*seq)->size; i++) {
    if ((*seq)->elements[i] != NULL) {
      release_record(&(*seq)->elements[i]);
    }
  }
  free(*seq);
  *seq = NULL;
}

/**
 * @brief クイックソート用の専用の構造体にしてます.汎用性無視してます.
 */
struct stack_data {
  int left;
  int right;
};

/**
 * @brief スタックの配列とメタ情報を保持する構造体.
 */
struct stack {
  int sp;                              /** 配列の長さ. */
  int size;                            /** 配列がとれる最大長. */
  stack_data elements[MAX_STACK_SIZE]; /** int型で表された配列. */
};

/**
 * @brief stackの初期化.
 * @return 初期化されたstackのポインタ.
 */
stack *init_stack() {
  stack *s = (stack *)malloc(sizeof(stack));
  s->sp = 0;
  s->size = MAX_STACK_SIZE;
  return s;
}

/**
 * @brief stackのメモリを解放.
 * @param[in] stack メモリを解放するstackのポインタ.
 */
void release_stack(stack **stack) {
  (*stack)->sp = 0;
  free(*stack);
  *stack = NULL;
}

/**
 * @brief valをスタックにプッシュする.
 * @param[in] stack プッシュする配列をもつstackのポインタ.
 * @param[in] data stack_data型のプッシュするデータ.
 */
void push_stack(stack *stack, int left, int right) {
  if (stack->sp >= stack->size) {
    printf("No more element can be pushed into the stack.\n");
    return;
  }

  stack->elements[stack->sp].left = left;
  stack->elements[stack->sp].right = right;
  stack->sp++;
}

/**
 * @brief スタックの先頭をポップし,stackを更新する.
 * @param[in] stack ポップする配列をもつstackのポインタ.
 * @param[in] data ポップした値の受けてとなるstack_data型のポインタ.
 */
void pop_stack(stack *stack, int *left, int *right) {
  if (stack->sp == 0) {
    printf("No more element can be popped from the elements.\n");
    return;
  }

  stack->sp--;
  *left = stack->elements[stack->sp].left;
  *right = stack->elements[stack->sp].right;
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
  return is_record_a_larger_than_b(seq->elements[a], seq->elements[b]);
}

/**
 * @brief 2つのインデックスでのkeyを評価し,[a]>[b]の場合スワップを決定する.
 * @param[in] records 評価するインデックスが指すrecordのポインタの配列.
 * @param[in] a 評価するインデックスのなかで最小(a<b).
 * @param[in] b 評価するインデックスのなかで最大(a<b).
 */
void compare_and_swap_records(record **records, int a, int b) {
  if (is_a_larger_than_b_records(records, a, b)) {
    SWAP(record *, records[a], records[b])
  }
}

/**
 * @brief 2つのインデックスでのkeyを評価し,[a]>[b]の場合スワップを決定する.
 * @param[in] seq 評価するインデックスが指すrecordのポインタの配列をもつsequenceのポインタ.
 * @param[in] a 評価するインデックスのなかで最小(a<b).
 * @param[in] b 評価するインデックスのなかで最大(a<b).
 */
void compare_and_swap_sequence(sequence *seq, int a, int b) {
  if (is_a_larger_than_b_sequence(seq, a, b)) {
    SWAP(record *, seq->elements[a], seq->elements[b])
  }
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

  if (seq->elements[seq->elements_length] == NULL) {
    seq->elements[seq->elements_length] = rec;
  } else {
    release_record(&seq->elements[seq->elements_length]);
    printf("The inserting index was dirty, so it was cleaned.\n");

    seq->elements[seq->elements_length] = rec;
  }
  seq->elements_length++;
}

/**
 * @brief sequenceのkey==targetを削除する.
 * @param[in] seq key==targetを削除するsequenceのポインタ.
 * @param[in] target 削除するキー.
 */
void delete_target_record(sequence *seq, int target) {
  bool found;
  int target_index;
  binary_search_existence_and_index(seq, target, &found, &target_index);
  if (found) {
    release_record(&seq->elements[target_index]);
    for (int i = target_index; i < seq->elements_length - 1; i++) {
      seq->elements[i] = seq->elements[i + 1];
    }
    seq->elements[seq->elements_length - 1] = NULL;
    seq->elements_length--;
  } else {
    printf("ERROR: Target was not found, and cannot be deleted.\n");
  }
}

/**
 * @brief 二分探索でsequenceにtargetが存在するか調べ,その時による真理値とインデックスを返す.
 * @param[in] tab targetが存在するか調べるsequence,整列になっている.
 * @param[in] target 調べるキー.
 * @param[out] found targetがtab内に存在するかの真理値.
 * @param[out] index sequenceのなかで,target以上であり最小のキー.
 */
void binary_search_existence_and_index(sequence *seq, int target, bool *found, int *index) {
  int ng = -1;
  int ok = seq->elements_length;
  int mid;
  while (ok - ng > 1) {
    mid = (ok + ng) / 2;
    if (target <= seq->elements[mid]->key) {
      ok = mid;
    } else {
      ng = mid;
    }
  }

  *found = ok == seq->elements_length ? false : target == seq->elements[ok]->key;
  *index = ok;
}

/**
 * @brief 単純なソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void simple_sort(sequence *seq) {
  for (int i = 0; i < seq->elements_length - 1; i++) {
    for (int j = i + 1; j < seq->elements_length; j++) {
      compare_and_swap_records(seq->elements, i, j);
    }
  }
}

/**
 * @brief バブルソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void bubble_sort(sequence *seq) {
  for (int i = 0; i < seq->elements_length - 1; i++) {
    for (int j = seq->elements_length - 1; j > i; j--) {
      compare_and_swap_sequence(seq, j - 1, j);
    }
  }
}

/**
 * @brief 選択ソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void selection_sort(sequence *seq) {
  int min_pos;
  for (int i = 0; i < seq->elements_length - 1; i++) {
    min_pos = i;
    for (int j = i + 1; j < seq->elements_length; j++) {
      if (is_a_larger_than_b_records(seq->elements, min_pos, j)) {
        min_pos = j;
      }
    }
    SWAP(record *, seq->elements[i], seq->elements[min_pos])
  }
}

/**
 * @brief 挿入ソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void insertion_sort(sequence *seq) {
  record *inserting_record;
  int j = 0;

  for (int i = 1; i < seq->elements_length; i++) {
    inserting_record = seq->elements[i];

    /**
     * long unsigned int / intのためj>=0をj!=-1に
     * 番兵にしていないため,jに関する条件式が必要.
     * 番兵にしたければ:
     * 1.挿入などを1オリジンにしてインデックス0を番兵に使えるようにする.
     * 2.末尾を番兵として,符号付きintにして-1のmodularをとって配列の末尾のインデックスをとる.
     * 3.全部を一個後ろにシフトする.
     * また番兵を用いる事を考えるならば,
     * MAX_RECORDS_SIZEの定義に+1をし,番兵分をつくる.
     * */
    for (j = i - 1; j != (int)-1; j--) {
      /* 挿入するrecord->key以上かつ最小のインデックスを探すまで,後ろにシフト*/
      if (is_record_a_larger_than_b(seq->elements[j], inserting_record)) {
        seq->elements[j + 1] = seq->elements[j];
        continue;
      } else {
        break;
      }
    }
    seq->elements[j + 1] = inserting_record;
  }
}

/**
 * @brief シェルソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void shell_sort(sequence *seq) {
  int h = 0, i = 0, j = 0;
  record *inserting_record = NULL;
  while (h < seq->elements_length - 1) {
    h = 3 * h + 1;
  }
  while (h > 1) {
    h = h / 3;
    /* h関係以外挿入ソートと同様にしている.*/
    for (i = h; i < seq->elements_length; i++) {
      inserting_record = seq->elements[i];

      /* デクリメント量が1からhになったため,hにそった条件式にしている.*/
      for (j = i - h; j != (i % h - h); j = j - h) {
        if (is_record_a_larger_than_b(seq->elements[j], inserting_record)) {
          seq->elements[j + h] = seq->elements[j];
          continue;
        } else {
          break;
        }
      }
      seq->elements[j + h] = inserting_record;
    }
  }
}

/**
 * @brief クイックソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void quick_sort(sequence *seq) {
  int left = 0, right = seq->elements_length - 1;

  printf("Using version: " STRINGFY(q_sort_definitive) "\n");
  q_sort_definitive(seq, left, right);
}

/**
 * @brief クイックソート.
 * @param[in] seq ソートするsequenceのポインタ.
 * @param[in] left 処理を行う塊のなかで最も小さいインデックス.
 * @param[in] right 処理を行う塊のなかで最も大きいインデックス.
 */
void q_sort_simple(sequence *seq, int left, int right) {
  if (left < right) {
    int pivot_index = (left + right) / 2;
    record *pivot_record = seq->elements[pivot_index];
    seq->elements[pivot_index] = seq->elements[left];
    /**
     * 分割の枢軸となるrecord*(値を持つモノ)をseqから除き,
     * 臨時として枢軸があったインデックスにleftでのrecord*をseqにいれる.
     *
     * leftとpivot_indexでは同じrecord*となる.
     * 自明だが,
     * leftが左側に所属するのであれば,pivot_indexも左側に,
     * leftが右側に所属するのであれば,pivot_indexも右側に所属する.
     * 感覚としてはpivot_indexが指すrecordを追い,そこにpivot_recordを戻すべきだと思うかもしれないが,
     * 同じモノであり,移動しないleftの方でも上の事を考えると大丈夫だと分かる.
     *
     * 最後に左側の最後のインデックスが指すrecordとSWAPし,
     * 交換された後のインデックス(左側の最後のインデックス)にpivot_recordで上書きする.
     */
    int left_land_last_index = left;
    for (int i = left + 1; i <= right; i++) {
      if (is_record_a_larger_than_b(pivot_record, seq->elements[i])) {
        /**
         * left_land_last_index++;を先にしてleft_land_last_index + 1をleft_land_last_indexにしても良い.
         * このように実装が冗長になっているのは理解しやすくするためである.
         * この実装の場合,left_land_last_indexの値は常にpivot_indexによって分けられる左側のインデックスを指すようになる.
         * そうするとSWAPの所は,左側の条件を満たさない最初のインデックスと左側を満たすインデックスを交換していると簡単に分かる.
         * 後は左側の最後のインデックスを指すようにインクリメントをし,更新している.
         */
        SWAP(record *, seq->elements[left_land_last_index + 1], seq->elements[i])
        left_land_last_index++;
      }
    }
    /* すべてのiでpivotの値より大きい時,左側は存在しない.*/
    /* すべてのiでpivotの値より小さい時,右側は存在しない.*/

    seq->elements[left] = seq->elements[left_land_last_index];
    pivot_index = left_land_last_index;
    seq->elements[pivot_index] = pivot_record;
    // left_land_last_index--;

    q_sort_simple(seq, left, pivot_index != left ? pivot_index - 1 : left);
    q_sort_simple(seq, pivot_index != right ? pivot_index + 1 : right, right);
  }
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
    record *pivot_record = seq->elements[pivot_index];
    int i = left, j = right;
    do {
      for (; is_record_a_larger_than_b(pivot_record, seq->elements[i]); ++i)
        ;
      for (; is_record_a_larger_than_b(seq->elements[j], pivot_record); --j)
        ;
      if (i <= j) {
        SWAP(record *, seq->elements[i], seq->elements[j])
        i++;
        j = (j - 1) % (int)-1;
      }
    } while (i <= j);

    q_sort_refined(seq, left, j);
    q_sort_refined(seq, i, right);
  }
}

/**
 * @brief クイックソート.
 * @param[in] seq ソートするsequenceのポインタ.
 * @param[in] left 処理を行う塊のなかで最も小さいインデックス.
 * @param[in] right 処理を行う塊のなかで最も大きいインデックス.
 */
void q_sort_definitive(sequence *seq, int whole_left, int whole_right) {
  // right - left < limitになったときにinsertionを用いる.
  int q_sort_limit = 20;
  stack *s = init_stack();
  push_stack(s, whole_left, whole_right);

  int left, right, sp;
  record *w1, *w2, *w3, *pivot_record, *w;
  int center;
  int i, j;
  do {
    pop_stack(s, &left, &right);
    while (right - left >= q_sort_limit) {
      center = (left + right) / 2;
      w1 = seq->elements[left];
      w2 = seq->elements[right];
      w3 = seq->elements[center];
      seq->elements[center] = seq->elements[left + 1];
      if (is_record_a_larger_than_b(w1, w2)) {
        SWAP(record *, w1, w2)
      }
      if (is_record_a_larger_than_b(w2, w3)) {
        SWAP(record *, w2, w3)
        if (is_record_a_larger_than_b(w1, w2)) {
          SWAP(record *, w1, w2)
        }
      }
      seq->elements[left] = w1;
      seq->elements[right] = w3;
      seq->elements[left + 1] = w2;

      //以降refinedと同じ
      pivot_record = w2;
      i = left + 1;
      j = right - 1;
      do {
        for (; is_record_a_larger_than_b(pivot_record, seq->elements[i]); ++i)
          ;
        for (; is_record_a_larger_than_b(seq->elements[j], pivot_record); --j)
          ;
        if (i <= j) {
          SWAP(record *, seq->elements[i], seq->elements[j])
          i++;
          j--;
        }
      } while (i <= j);

      //短い方を選び,stackに入る数を減らす
      if (j - left <= right - i) {
        push_stack(s, i, right);
        right = j;
      } else {
        push_stack(s, left, j);
        left = i;
      }
    }
  } while (s->sp > 0);

  insertion_sort(seq);
}

/**
 * @brief ヒープソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void heap_sort(sequence *seq) {
  for (int i = seq->elements_length / 2 - 1; i != (int)-1; i--) {
    down_heap(seq, i, seq->elements_length - 1);
  }

  for (int i = seq->elements_length - 1; i > 0;) {
    SWAP(record *, seq->elements[0], seq->elements[i])
    down_heap(seq, 0, --i);
  }
}

/**
 * @brief ヒープソート,縦方向の順序を確認し更新.
 * @param[in] seq 更新するsequenceのポインタ.
 * @param[in] target_node_index 縦方向の順序を確認する部分木の根のノード.
 * @param[in] end_node_index 更新対象の末尾のインデックス.
 */
void down_heap(sequence *seq, int target_node_index, int end_node_index) {
  while (true) {
    /**
     *                0
     *       1                  2
     *   3       4         5         6
     * 7   8   9   10   11   12   13   14
     * 左部分木のインデックス
     */
    int comparing_node_index = 2 * target_node_index + 1;
    if (comparing_node_index > end_node_index) {
      break;
    }
    // comparing_node_index == end_node_indexの時に右部分木は存在しない.
    if (comparing_node_index != end_node_index) {
      if (is_record_a_larger_than_b(seq->elements[comparing_node_index + 1], seq->elements[comparing_node_index])) {
        //右部分木に
        comparing_node_index++;
      }
    }
    if (is_record_a_larger_than_b(seq->elements[target_node_index], seq->elements[comparing_node_index])) {
      break;
    }
    SWAP(record *, seq->elements[target_node_index], seq->elements[comparing_node_index])
    target_node_index = comparing_node_index;
  }
}

/**
 * @brief マージソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void merge_sort(sequence *seq) {
  sequence *new_seq = init_sequence();
  new_seq->elements_length = seq->elements_length;
  int segment_size = 1;
  while (segment_size < seq->elements_length) {
    merge_seqs(segment_size, seq, new_seq);
    merge_seqs(2 * segment_size, new_seq, seq);
    segment_size *= 4;
  }
}

/**
 * @brief マージソート,複数の塊を順序を判断し合成.
 * @param[in] segment_size 区分のサイズ.
 * @param[in] from 合成されるsequenceのポインタ.
 * @param[in] into 合成した結果を格納するsequenceのポインタ.
 */
void merge_seqs(int segment_size, sequence *from, sequence *into) {
  int start_index = 0;
  int i, j, k;
  int i_end, j_end;

  while (start_index < from->elements_length) {
    i = start_index;
    j = start_index + segment_size;
    k = start_index;

    i_end = MIN(start_index + segment_size - 1, from->elements_length - 1);
    j_end = MIN(start_index + 2 * segment_size - 1, from->elements_length - 1);

    while (i <= i_end && j <= j_end) {
      if (is_record_a_larger_than_b(from->elements[j], from->elements[i])) {
        put(from, into, &i, &k);
      } else {
        put(from, into, &j, &k);
      }
    }

    while (i <= i_end) {
      put(from, into, &i, &k);
    }

    while (j <= j_end) {
      put(from, into, &j, &k);
    }

    start_index = start_index + 2 * segment_size;
  }
}

/**
 * @brief マージソート,合成途中の挿入.
 * @param[in] from 挿入するコンテンツをもつsequenceのポインタ.
 * @param[in] into 挿入されるsequenceのポインタ.
 * @param[in] from_inputting_index 挿入するコンテンツが存在するインデックス.
 * @param[in] into_inputted_index 挿入するインデックス.
 */
void put(sequence *from, sequence *into, int *from_inputting_index, int *into_inputted_index) {
  into->elements[*into_inputted_index] = from->elements[*from_inputting_index];
  (*from_inputting_index)++;
  (*into_inputted_index)++;
}

/**
 * @brief カウンソート(サンプル).
 * @param[in] seq ソートするsequenceのポインタ.
 * @param[in] m seq内の最大キー<mを指定,keyをインデックスとする際の配列の大きさ.
 */
void count_sort(sequence *seq, int m) {
  int counter[m];
  record *b[seq->elements_length];
  for (int i = 0; i < m; ++i) {
    counter[i] = 0;
  }
  for (int i = 0; i < seq->elements_length; ++i) {
    counter[seq->elements[i]->key]++;
  }
  for (int i = 0; i < m; ++i) {
    counter[i] += (i > 0 ? counter[i - 1] : 0);
  }
  for (int i = seq->elements_length - 1; i != (int)-1; --i) {
    int j = counter[seq->elements[i]->key];
    j = (j - 1) % (int)-1;
    b[j] = seq->elements[i];
    counter[seq->elements[i]->key] = j;
  }

  for (int i = 0; i < seq->elements_length; ++i) {
    seq->elements[i] = b[i];
  }
}

/**
 * @brief ビンソート(サンプル).
 * @param[in] seq ソートするsequenceのポインタ.
 * @param[in] m seq内の最大キー<mを指定,keyをインデックスとする際の配列の大きさ.
 */
void bin_sort(sequence *seq, int m) {
  int list_head[m];
  int next[seq->elements_length];
  record *b[seq->elements_length];
  for (int i = 0; i < m; ++i) {
    //例外とする-1で初期化.指定できないインデックスを入れておきたい.
    list_head[i] = -1;
  }
  //昇順,降順の違い.同値のkeyの順序が変わることを気にするかどうか(降順の場合変わらない).
  for (int i = seq->elements_length - 1; i != (int)-1; --i) {
    /**
     * nextは,
     * 初めてであれば-1,二回目以降であれば他に存在するjのseqでのインデックス.
     * list_headは,
     * 初期値-1,更新されるときに指定された値のseqでのインデックス.
     * list_headから鎖が出ているイメージ,
     * 鎖自体はnextという配列で管理している.
     * -1で末端を示している.
     */
    int j = seq->elements[i]->key;
    next[i] = list_head[j];
    list_head[j] = i;
  }

  int i = 0;
  for (int j = 0; j < m; ++j) {
    int p = list_head[j];
    while (p != -1) {
      b[i] = seq->elements[p];

      p = next[p];
      ++i;
    }
  }

  for (int i = 0; i < seq->elements_length; ++i) {
    seq->elements[i] = b[i];
  }
}

/**
 * @brief 基数ソート.
 * @param[in] seq ソートするsequenceのポインタ.
 * @param[in] radix_size 基数の大きさ,10進数の桁だけでなく16なども指定可能.
 */
void radix_sort(sequence *seq, int radix_size) {
  int max_value = find_max_value(seq);

  int next[seq->elements_length];
  int radix_digits_head[radix_size];
  record *b[seq->elements_length];

  for (int radix_digit = 1; radix_digit <= max_value; radix_digit *= radix_size) {
    for (int i = 0; i < radix_size; ++i) {
      radix_digits_head[i] = -1;
    }

    //複数回評価をおこなうため,降順である必要がある.昇順にすると同値での順番が逆になり整列されない.
    for (int i = seq->elements_length - 1; i != (int)-1; --i) {
      // listheadのインデックスを求める所がビンソートと異なる.
      int radix_digit_value = (seq->elements[i]->key / radix_digit) % radix_size;
      next[i] = radix_digits_head[radix_digit_value];
      radix_digits_head[radix_digit_value] = i;
    }

    int i = 0;
    for (int j = 0; j < radix_size; ++j) {
      int p = radix_digits_head[j];
      while (p != (int)-1) {
        b[i] = seq->elements[p];

        p = next[p];
        ++i;
      }
    }

    for (int i = 0; i < seq->elements_length; ++i) {
      seq->elements[i] = b[i];
    }
  }
}

int find_max_value(sequence *seq) {
  int max = 0;
  for (int i = 0; i < seq->elements_length; ++i) {
    max = MAX(max, seq->elements[i]->key);
  }
  return max;  //(int)(log10(max) / log10(radix_size) + 1);
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
      print_record(seq->elements[i]);
    }
    printf("================================\n");
    printf("Sequence size: (%zu/%zu)\n", seq->elements_length, seq->size);
    printf("================================\n");
    printf("\n");
  }
}

/**
 * @brief binary_search_existence_and_index確認用プリント関数.
 * @param[in] tab targetが存在するか調べるsequence.
 * @param[in] target 調べるキー.
 */
void print_search_existence_and_index(sequence *seq, int target) {
  printf("\n");
  printf("================================\n");

  bool found = false;
  int target_index;
  binary_search_existence_and_index(seq, target, &found, &target_index);
  printf("\"%zu\" was %s\n", target, found ? "FOUND." : "NOT FOUND.");
  if (found) {
    printf("The content of the target is...\n");
    print_record(seq->elements[target_index]);
  }
  printf("================================\n");
  printf("\n");
}

/**
 * @brief cliからrecordの内容を読み取りrecordに格納.
 * @return cliから読み取った内容で生成したrecordのポインタ
 */
record *cli_record() {
  record *rec;
  int key = -1;
  char field[MAX_FIELD_SIZE];

  printf("Type in a key >= 0 and a field. (example: \"10001 BBB\")\n");
  printf(STRINGFY(MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER) "=" DEF_STRINGFY(MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER) "\n");

  while (true) {
    scanf("%zu", &key);
    if (key == (int)-1) {
      getchar();
      printf("ERROR: Try again from the key.\n");
      continue;
    } else {
      break;
    }
  }
  scanf("%" DEF_STRINGFY(MAX_FIELD_SIZE_WITHOUT_NULL_CHARACTER) "s%*[^\n]", field);

  rec = init_record(key, field);
  return rec;
}

/**
 * @brief cliからrecordの内容を読み取り,tableの末尾に挿入.
 * @param[in] seq スキャンしたrecordを挿入するseqのポインタ.
 */
void cli_insert(sequence *seq) {
  printf("ENETER A RECORD THAT WILL BE INSERTED.\n");
  record *scanned_rec = NULL;
  while (true) {
    // CLIからの入力を受け付け,入力されたデータをscanned_recに代入.
    scanned_rec = cli_record();
    // 同じキーが既に存在しているか確認.
    bool found;
    int dummy;
    binary_search_existence_and_index(seq, scanned_rec->key, &found, &dummy);
    if (found) {
      printf("The key is already used.\n");
      continue;
    } else {
      break;
    }
  }
  // seqに挿入.
  insert_tail(seq, scanned_rec);
  // 挿入したrecordを表示.
  printf("Record :\n");
  print_record(scanned_rec);
  printf("was inserted.\n");
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
    if (former > seq->elements[i]->key) {
      sorted = false;
    } else {
      former = seq->elements[i]->key;
    }
  }

  printf("Sequence is ... %s\n", sorted ? "in sort." : "not in sort.");
}

int main() {
  /*
  int keys[MAX_RECORDS_SIZE];
  for (int i = 0; i < MAX_RECORDS_SIZE; i++)
  {
    keys[i] = i;
  }

  fisher_yates_shuffle(keys, sizeof(keys) / sizeof(int));

  sequence* seq = init_sequence();
  for (int i = 0; i < MAX_RECORDS_SIZE; i++)
  {
    insert_tail(seq, init_record(keys[i], (char *)"AAA"));
  }

  printf("Record size: %d\n", MAX_RECORDS_SIZE);
  printf("Using algorithm: " STRINGFY(heap_sort) "\n");
  run_sort_algorithm(seq, heap_sort);
  */

  int m = 15;
  int keys[m];

  for (int i = 0; i < m; ++i) {
    //適当にわざと重複させている
    keys[i] = i % 10;
  }

  fisher_yates_shuffle(keys, sizeof(keys) / sizeof(int));

  sequence *seq = init_sequence();
  for (int i = 0; i < m; ++i) {
    insert_tail(seq, init_record(keys[i], (char *)"AAA"));
  }

  print_sequence(seq);

  // count_sort(seq, find_max_value(seq) + 1);
  bin_sort(seq, find_max_value(seq) + 1);
  // radix_sort(seq, 10);

  print_sequence(seq);

  release_sequence(&seq);
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
