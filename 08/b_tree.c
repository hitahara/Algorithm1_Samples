#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct record record;
typedef struct key_branch_pair key_branch_pair;
typedef struct internal_data internal_data;
typedef struct b_node b_node;
typedef b_node *b_tree;

record *init_record(int key, const char *field);
key_branch_pair *init_pair();
internal_data *init_internal_data();
b_node *init_b_node_leaf(record *rec);
b_node *init_b_node_internal();

void shift_b_node_internal(b_node *shifting_node, int min_condition_key);
void b_node_ins(b_node **bn, record *inserting_rec, key_branch_pair **parent_pair, bool *parent_expanded);
void b_node_insert(b_tree *b_rt, record *inserting_rec);

void binary_search_existence_and_index(b_node *n, int target, bool *found, int *index);

void print_record(record *rec);
void print_leaf_node(b_node *n);
void print_internal_node(b_node *n);
void print_tree_recursion(b_tree p, int depth);
void print_tree(b_tree rt);
void print_search_node(b_tree rt, int target);

record *cli_record();
void cli_insert(b_tree *rt);

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
 * @brief ノードで格納できるレコード数.
 */
#define M 5

/**
 * @brief レコード：keyとfieldをもった構造体
 */
struct record {
  int key;                      /** int型のキー. key==-1==2^64-1を例外処理に用いている. */
  char field[MAX_FIELD_MEMORY]; /** データを保持するchar型の配列. */
};

/**
 * @brief B木の端かどうかを格納.
 */
typedef enum node_type {
  INTERNAL,
  LEAF,
} node_type;

/**
 * @brief B木の枝間の区間を決めるkeyと枝先のペアの構造体.
 * @details
 * key:     [0]  [1]  [...]  [i]  [...]
 * branch:   [0]  [1]  [...]  [i]  [...]
 * key_branch_pair[0]のkeyは走査に使われない.
 * 挿入の分割でノードの親へkeyを渡す際に使用される.
 */
struct key_branch_pair {
  int key;        /** 区間を示す値. */
  b_node *branch; /** 枝を示すnodeのポインタ. */
};

/**
 * @brief ノードのINTERNALデータを格納する構造体.
 * @details 構造体名示さない状態で書いていたが,
 * ポインタで書いているため,メモリが占有されない.
 * そのためmallocを書かなければいけなく,
 * 構造体が指定可能でないといけないため,
 * 定義されている.
 * 逆にポインタでなければ直書きでも良い.
 */
struct internal_data {
  int count;
  key_branch_pair pairs[M];
};

/**
 * @brief B木のノード.
 */
struct b_node {
  node_type tag;
  /* 変数internalとleafが共有の領域を使う. */
  union {
    internal_data *internal;
    record *leaf;
  };
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

/**
 * @brief key_branch_pairの初期化.
 * @return 初期化されたkey_branch_pairのポインタ.
 */
key_branch_pair *init_pair() {
  key_branch_pair *p = (key_branch_pair *)malloc(sizeof(key_branch_pair));
  if (p == NULL) {
    fprintf(stderr, "ERROR: Not enough memory for new node.\n");
    exit(1);
  }
  return p;
}

/**
 * @brief internal_dataの初期化.
 * @return 初期化されたinternal_dataのポインタ.
 */
internal_data *init_internal_data() {
  internal_data *i = (internal_data *)malloc(sizeof(internal_data));
  if (i == NULL) {
    fprintf(stderr, "ERROR: Not enough memory for new node.\n");
    exit(1);
  }
  return i;
}

/**
 * @brief INTERNALであるb_nodeの初期化.
 * @return 初期化されたb_nodeのポインタ.
 */
b_node *init_b_node_internal() {
  b_node *bn = (b_node *)malloc(sizeof(b_node));
  if (bn == NULL) {
    fprintf(stderr, "ERROR: Not enough memory for new node.\n");
    exit(1);
  }
  bn->tag = INTERNAL;
  bn->internal = init_internal_data();
  bn->internal->count = 0;

  return bn;
}

/**
 * @brief LEAFであるb_nodeの初期化.
 * @param[in] rec LEAFに入るrecord.
 * @return 初期化されたb_nodeのポインタ.
 */
b_node *init_b_node_leaf(record *rec) {
  b_node *bn = (b_node *)malloc(sizeof(b_node));
  if (bn == NULL) {
    fprintf(stderr, "ERROR: Not enough memory for new node.\n");
    exit(1);
  }
  bn->tag = LEAF;
  bn->leaf = rec;
  return bn;
}

/**
 * @brief b_node内のデータをずらす.
 * @param[in,out] shifting_node ずらすb_nodeのポインタ.
 * @param[in] min_condition_key 下限となるインデックス.
 */
void shift_b_node_internal(b_node *shifting_node, int min_condition_key) {
  for (int i = shifting_node->internal->count + ((shifting_node->internal->count == M) ? -1 : 0); i >= min_condition_key + 1; i--) {
    shifting_node->internal->pairs[i] = shifting_node->internal->pairs[i - 1];
  }
}

/**
 * @brief b_nodeにrecordを挿入する.
 * @param[in,out] bn inserting_recを挿入するb_nodeのポインタのアドレス.
 * @param[in] inserting_rec 挿入するrecordのポインタ.
 * @param[out] parent_pair 呼び出し元で使用される,新規のkey_branch_pairの設定.
 * @param[out] parent_expanded 呼び出し元で使用される,横に挿入され広がるのか.
 */
void b_node_ins(b_node **bn, record *inserting_rec, key_branch_pair **parent_pair, bool *parent_expanded) {
  *parent_expanded = false;
  if ((*bn)->tag == LEAF) {
    if (inserting_rec->key == (*bn)->leaf->key) {
      fprintf(stderr, "ERROR: The key already exists in the tree.\n");
      exit(1);
    } else {
      b_node *new_node = init_b_node_leaf(inserting_rec);
      if (inserting_rec->key < (*bn)->leaf->key) {
        b_node *swap_tmp = *bn;
        *bn = new_node;
        new_node = swap_tmp;
      }
      (*parent_pair)->key = new_node->leaf->key;
      (*parent_pair)->branch = new_node;
      *parent_expanded = true;
    }
  } else {
    bool dummy;
    int inserting_index;
    binary_search_existence_and_index(*bn, inserting_rec->key, &dummy, &inserting_index);
    inserting_index--;

    key_branch_pair *new_pair = init_pair();
    bool expands;

    b_node_ins(&(*bn)->internal->pairs[inserting_index].branch, inserting_rec, &new_pair, &expands);
    inserting_index++;

    if (expands) {
      if ((*bn)->internal->count < M) {
        shift_b_node_internal(*bn, inserting_index);
        (*bn)->internal->pairs[inserting_index] = *new_pair;
        (*bn)->internal->count++;
      } else {
        // split
        // split_index以降が新しく生成されたノードへ
        b_node *split_result_node = init_b_node_internal();
        int split_index = (M + 1) / 2;

        if (inserting_index >= split_index) {
          for (int i = split_index; i < inserting_index; i++) {
            split_result_node->internal->pairs[i - split_index] = (*bn)->internal->pairs[i];
          }
          split_result_node->internal->pairs[inserting_index - split_index] = *new_pair;
          //挿入したため,1インデックス分後ろを指定している.
          for (int i = inserting_index; i < M; i++) {
            split_result_node->internal->pairs[i - split_index + 1] = (*bn)->internal->pairs[i];
          }
        } else {
          for (int i = split_index - 1; i < M; i++) {
            split_result_node->internal->pairs[i - (split_index - 1)] = (*bn)->internal->pairs[i];
          }
          printf("KEY %d\n", new_pair->branch->leaf->key);

          shift_b_node_internal(*bn, inserting_index);  //余計にsplit_index以降,(*bn)->internal->countまで走ってしまってる.

          (*bn)->internal->pairs[inserting_index] = *new_pair;
        }

        (*bn)->internal->count = split_index;
        split_result_node->internal->count = M + 1 - (*bn)->internal->count;

        (*parent_pair)->key = split_result_node->internal->pairs[0].key;
        (*parent_pair)->branch = split_result_node;

        *parent_expanded = true;
      }
    }
  }
}

/**
 * @brief b_nodeにrecordを挿入する.
 * @details 根付近の特殊な場合をケアする,根ノードでB木は高さ方向に成長する.
 * @param[in,out] b_rt inserting_recを挿入するb_nodeのポインタのアドレス.
 * @param[in] inserting_rec 挿入するrecordのポインタ.
 */
void b_node_insert(b_tree *b_rt, record *inserting_rec) {
  if (*b_rt == NULL) {
    *b_rt = init_b_node_leaf(inserting_rec);
  } else {
    key_branch_pair *new_pair = init_pair();
    bool expands;
    b_node_ins(b_rt, inserting_rec, &new_pair, &expands);

    if (expands) {
      b_node *new_rt = init_b_node_internal();
      new_rt->internal->count = 2;
      new_rt->internal->pairs[0].branch = *b_rt;
      new_rt->internal->pairs[1] = *new_pair;
      *b_rt = new_rt;
    }
  }
}

/**
 * @brief 二分探索でb_nodeにtargetが存在するか調べ,その時に沿う真理値とインデックスを返す.
 * @param[in] n targetが存在するか調べるtable,整列になっている.
 * @param[in] target 調べるキー.
 * @param[out] found targetがtab内に存在するかの真理値.
 * @param[out] index targetが存在する場合キーのインデックスを指す,無い場合はtarget以上の最小キー.
 */
void binary_search_existence_and_index(b_node *n, int target, bool *found, int *index) {
  if (n->tag == LEAF) {
    *found = false;
    *index = -1;
    return;
  }
  int ng = 0;
  int ok = n->internal->count;  // tab->records[tab->records_length - 1].key < targetのことを考えて
  int mid;
  while (ok - ng > 1) {
    mid = (ok + ng) / 2;
    if (target <= n->internal->pairs[mid].key) {
      ok = mid;
    } else {
      ng = mid;
    }
  }

  *found = ok == n->internal->count ? false : target == n->internal->pairs[ok].key;
  *index = ok;
}

/**
 * @brief B木に存在するか探索.
 * @param[in] rt 探索するB木の根を指すtree.
 * @param[in] target 探索する対象のキー.
 * @param[out] found targetがtab内に存在するかの真理値.
 * @param[out] target_rec 探索する対象のキーを持ったrecord.
 */
void search_existence_and_record(b_tree rt, int target, bool *found, record **target_rec) {
  *found = false;
  b_tree p = rt;
  // for文の最初でi==0になる
  for (int i = 0; p != NULL && p->tag != LEAF; i = 0) {
    binary_search_existence_and_index(p, target, found, &i);
    if (*found) {
      p = p->internal->pairs[i].branch;
    } else {
      p = p->internal->pairs[i - 1].branch;
    }
  }

  if (p->leaf->key == target) {
    *found = true;
    *target_rec = p->leaf;
  }
}

/**
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record *rec) {
  printf("[%06zu, %s]\n", rec->key, rec->field);
}
/**
 * @brief node確認用プリント関数.
 * @param[in] node プリントするnodeのポインタ.
 */
void print_leaf_node(b_node *n) {
  if (n->tag == INTERNAL) {
    return;
  }
  print_record(n->leaf);
}
/**
 * @brief node確認用プリント関数.
 * @param[in] node プリントするnodeのポインタ.
 */
void print_internal_node(b_node *n) {
  if (n->tag == LEAF) {
    return;
  }
  printf("[");
  for (int i = 1; i < n->internal->count; i++) {
    printf("%d, ", n->internal->pairs[i].key);
  }
  printf("]\n");
}

/**
 * @brief 二分探索木を再帰でプリント.
 * @param[in] p 操作する対象のtree.
 * @param[in] depth 再帰呼び出しの深さ.
 */
void print_tree_recursion(b_tree p, int depth) {
  if (p != NULL) {
    for (int i = 0; i < depth; i++) {
      printf("   ");
    }
    if (p->tag == INTERNAL) {
      print_internal_node(p);
      for (int i = 0; i < p->internal->count; i++) {
        print_tree_recursion(p->internal->pairs[i].branch, depth + 1);
      }
    } else {
      print_leaf_node(p);
    }
  }
}
/**
 * @brief 二分探索木をプリント.
 * @param[in] rt 操作する対象のtree.
 */
void print_tree(b_tree rt) {
  int depth = 0;
  printf("VISUALISING TREE\n");
  printf("================================\n");
  print_tree_recursion(rt, depth);
  printf("================================\n");
}

/**
 * @brief search_node確認用プリント関数.
 * @param[in] rt targetが存在するか調べるtree.
 * @param[in] target 調べるキー.
 */
void print_search_node(b_tree rt, int target) {
  bool found;
  record *target_rec = NULL;

  search_existence_and_record(rt, target, &found, &target_rec);

  printf("\"%d\" was %s\n", target, found ? "FOUND." : "NOT FOUND.");
  if (found) {
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

  while (key == -1) {
    scanf("%d", &key);
  }
  scanf("%" DEF_STRINGFY(MAX_FIELD_MEMORY) "s%*[^\n]", field);

  rec = init_record(key, field);

  return rec;
}

/**
 * @brief cliからrecordの内容を読み取り,tableの末尾に挿入.
 * @param[in] rt スキャンしたrecordを挿入するtreeのポインタ.
 */
void cli_insert(b_tree *rt) {
  printf("ENETER A RECORD THAT WILL BE INSERTED.\n");
  record *scanned_rec = NULL;
  while (true) {
    scanned_rec = cli_record();
    bool found;
    int dummy;
    binary_search_existence_and_index(*rt, scanned_rec->key, &found, &dummy);
    if (found) {
      printf("The key is already used.\n");
    } else {
      break;
    }
  }
  b_node_insert(rt, scanned_rec);
}

int main() {
  // こちらの可視化ツールも合わせて使ってみてください.
  //今回のB木の定義とは異なりますが,雰囲気がつかめると思います.
  // https://www.cs.usfca.edu/~galles/visualization/BTree.html
  b_tree rt = NULL;

  for (int i = 0; i < 15; i++) {
    b_node_insert(&rt, init_record(i, "AAAA"));
  }

  print_tree(rt);
  print_search_node(rt, 8);

  return 0;
}
