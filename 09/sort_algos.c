#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_RECORD_SIZE 10

#define SWAP(type, a, b)   \
    {                      \
        type swap_tmp = a; \
        a = b;             \
        b = swap_tmp;      \
    }

typedef struct record record;
typedef struct sequence sequence;

record *init_record(int key, char *field);
sequence *init_sequence();
void release_record(record **rec);
void release_sequence(sequence **seq);

bool is_record_a_larger_than_b(record *a, record *b);
bool is_a_larger_than_b(record **records, int a, int b);
void compare_and_swap(record **records, int a, int b);

void insert_tail(sequence *seq, record *rec);

void binary_search_existence_and_index(sequence *seq, int target, bool *found,
                                       int *index);

void simple_sort(sequence *seq);
void bubble_sort(sequence *seq);
void selection_sort(sequence *seq);
void insertion_sort(sequence *seq);

void print_record(record *rec);
void print_sequence(sequence *seq);
void print_search_existence_and_index(sequence *seq, int target);

record *cli_record();
void cli_insert(sequence *seq);
void run_sort_algorithm(sequence *seq, void (*sort_algorithm)(sequence *));

void fisher_yates_shuffle(int *array, int array_size);

/**
 * @brief レコード：keyとfieldをもった構造体
 */
struct record {
    int key; /** int型のキー. key==-1or2^64-1を例外処理に用いている*/
    char field[32]; /** データを保持するchar型の配列. */
};

/**
 * @brief 配列とそのメタデータを保持する構造体.
 */
struct sequence {
    int elements_length;               /** 配列の長さ. */
    int size;                          /** 配列がとれる最大長. */
    record *elements[MAX_RECORD_SIZE]; /** recordのポインタの配列. */
};

/**
 * @brief recordの初期化.
 * @param[in] key 初期化時に指定するキー.
 * @param[in] field 静的ならばconst char*,動的ならばchar*にしなければいけない,
 * サイズはMAX_FIELD_SIZEで定義.初期化時に指定するフィールド.
 * @return 初期化されたrecordのポインタ.
 */
record *init_record(int key, char *field) {
    record *rec = (record *)malloc(sizeof(record));
    if (rec == NULL) {
        fprintf(stderr, "ERROR: Not enough memory for new record.\n");
        exit(1);
    }

    rec->key = key;
    strcpy(rec->field, field);

    return rec;
}
/**
 * @brief sequenceの初期化.
 * @return 初期化されたsequenceのポインタ.
 */
sequence *init_sequence() {
    sequence *s = (sequence *)malloc(sizeof(sequence));
    s->elements_length = 0;
    s->size = MAX_RECORD_SIZE;
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
 * @brief 2つのreocrdをkeyで評価.
 * @param[in] a 評価するrecord.
 * @param[in] b 評価するrecord.
 * @return a->key > b->keyの場合:true,a->key > b->keyの場合:false.
 */
bool is_record_a_larger_than_b(record *a, record *b) { return a->key > b->key; }

/**
 * @brief 2つのインデックスでのkeyを評価.
 * @param[in] records 評価するインデックスが指すrecordのポインタの配列.
 * @param[in] a 評価するインデックスのなかで最小(a<b).
 * @param[in] b 評価するインデックスのなかで最大(a<b).
 * @return a>bの場合:true,a<bの場合:false.
 */
bool is_a_larger_than_b(record **records, int a, int b) {
    return is_record_a_larger_than_b(records[a], records[b]);
}

/**
 * @brief 2つのインデックスでのkeyを評価し,[a]>[b]の場合スワップを決定する.
 * @param[in] records 評価するインデックスが指すrecordのポインタの配列.
 * @param[in] a 評価するインデックスのなかで最小(a<b).
 * @param[in] b 評価するインデックスのなかで最大(a<b).
 */
void compare_and_swap(record **records, int a, int b) {
    if (is_a_larger_than_b(records, a, b)) {
        SWAP(record *, records[a], records[b])
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
 * @brief
 * 二分探索でsequenceにtargetが存在するか調べ,その時による真理値とインデックスを返す.
 * @param[in] tab targetが存在するか調べるsequence,整列になっている.
 * @param[in] target 調べるキー.
 * @param[out] found targetがtab内に存在するかの真理値.
 * @param[out] index sequenceのなかで,target以上であり最小のキー.
 */
void binary_search_existence_and_index(sequence *seq, int target, bool *found,
                                       int *index) {
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

    *found =
        ok == seq->elements_length ? false : target == seq->elements[ok]->key;
    *index = ok;
}

/**
 * @brief 単純なソート.
 * @param[in] seq ソートするsequenceのポインタ.
 */
void simple_sort(sequence *seq) {
    for (int i = 0; i < seq->elements_length - 1; i++) {
        for (int j = i + 1; j < seq->elements_length; j++) {
            compare_and_swap(seq->elements, i, j);
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
            compare_and_swap(seq->elements, j - 1, j);
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
            if (is_a_larger_than_b(seq->elements, min_pos, j)) {
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
         * MAX_RECORD_SIZEの定義に+1をし,番兵分をつくる.
         * */
        for (j = i - 1; j != (int)-1; j--) {
            /** 挿入するrecord->key以上かつ最小のインデックスを探すまで,後ろにシフト*/
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
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record *rec) {
    if (rec == NULL) {
        printf("ERROR: Record is NULL\n");
    } else {
        printf("[%d, %s]\n", rec->key, rec->field);
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
        printf("Sequence size: (%d/%d)\n", seq->elements_length, seq->size);
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
    printf("\"%d\" was %s\n", target, found ? "FOUND." : "NOT FOUND.");
    if (found) {
        printf("The content of the target is...\n");
        print_record(seq->elements[target_index]);
    }
    printf("================================\n");
    printf("\n");
}

void run_sort_algorithm(sequence *seq, void (*sort_algorithm)(sequence *)) {
    double start_clock, end_clock;

    start_clock = (double)clock();
    sort_algorithm(seq);
    end_clock = (double)clock();

    printf("CPU execution time: %.8lf s.\n",
           (end_clock - start_clock) / CLOCKS_PER_SEC);
}

int main() {
    int keys[MAX_RECORD_SIZE];
    for (int i = 0; i < MAX_RECORD_SIZE; i++) {
        keys[i] = i;
    }

    fisher_yates_shuffle(keys, sizeof(keys) / sizeof(int));

    sequence *seq = init_sequence();
    for (int i = 0; i < MAX_RECORD_SIZE; i++) {
        insert_tail(seq, init_record(keys[i], (char *)"AAAAAAAA"));
    }

    print_sequence(seq);
    printf("Using algorithm: bubble_sort\n");
    run_sort_algorithm(seq, bubble_sort);

    print_sequence(seq);
    print_search_existence_and_index(seq, 25);

    delete_target_record(seq, 25);

    print_sequence(seq);
    print_search_existence_and_index(seq, 25);

    release_sequence(&seq);
    print_sequence(seq);
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