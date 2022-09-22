#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
 * @brief データと二分木の2つの枝を保持する構造体.
 */
typedef struct node node;

/**
 * @brief nodeのポインタ.
 */
typedef node *tree;

/**
 * @brief nodeの左右の子の深さの偏り.
 */
typedef enum direction
{
  LEFT = 0,
  RIGHT = 1,
  BALANCED,
} direction;

/**
 * @brief レコード：keyとfieldをもった構造体
 */
typedef struct record
{
  size_t key;                   /** size_t型のキー. key==-1or2^64-1を例外処理に用いている*/
  char field[MAX_FIELD_MEMORY]; /** データを保持するchar型の配列. */
} record;

struct node
{
  record *rec;    /** keyとデータを格納するrecordのポインタ. */
  node *child[2]; /** nodeのchild左:0と右:1を指す. */
  direction bias; /** leftとright以下の深さの偏り. */
};

/**
 * @brief recordの初期化.
 * @param[in] key 指定するキー.
 * @param[in] field const char*,動的にするのであればchar*,サイズはMAX_FIELD_MEMORYで定義.
 * @return 初期化されたrecordのポインタ.
 */
record *init_record(size_t key, const char *field)
{
  if (strlen(field) > MAX_FIELD_MEMORY + 1)
  {
    fprintf(stderr, "ERROR: \"field\" is too large.\n");
    exit(1);
  }

  record *rec = (record *)malloc(sizeof(record));
  if (rec == NULL)
  {
    return NULL;
  }
  rec->key = key;
  strncpy(rec->field, field, MAX_FIELD_MEMORY);

  return rec;
}

/**
 * @brief nodeの初期化.
 * @return 初期化された,データやポインタが代入されていないnodeのポインタ.
 */
node *init_node(record *rec)
{
  node *c = (node *)malloc(sizeof(node));
  if (c == NULL)
  {
    return NULL;
  }
  c->rec = rec;
  c->child[LEFT] = NULL;
  c->child[RIGHT] = NULL;
  c->bias = BALANCED;
  return c;
}

/**
 * @brief 前順操作.
 * @param[in] p 操作する対象のnodeのポインタ.
 * @param[in] something 操作を実装した関数.
 */
void pre_order(node *p, void (*something)(node *))
{
  if (p != NULL)
  {
    something(p);
    pre_order(p->child[LEFT], something);
    pre_order(p->child[RIGHT], something);
  }
}

/**
 * @brief 間順操作.
 * @param[in] p 操作する対象のnodeのポインタ.
 * @param[in] something 操作を実装した関数.
 */
void in_order(node *p, void (*something)(node *))
{
  if (p != NULL)
  {
    in_order(p->child[LEFT], something);
    something(p);
    in_order(p->child[RIGHT], something);
  }
}

/**
 * @brief 後順操作.
 * @param[in] p 操作する対象のnodeのポインタ.
 * @param[in] something 操作を実装した関数.
 */
void post_order(node *p, void (*something)(node *))
{
  if (p != NULL)
  {
    post_order(p->child[LEFT], something);
    post_order(p->child[RIGHT], something);
    something(p);
  }
}

/**
 * @brief 引数のnodeを削除する.
 * @param[in] n 削除するnodeのポインタ.
 */
void dispose(node *n)
{
  free(n);
}
/**
 * @brief treeのメモリを解放.
 * @param[in] tab メモリを開放するtreeのポインタ.
 */
void release_tree(tree *rt)
{
  post_order(*rt, dispose);
  *rt = NULL;
}

/**
 * @brief 部分木を1重回転（挿入用）.
 * @param[in] rt 回転する部分木の根.
 * @param[in] inserted_dir 成長した部分木の方向.
 */
tree single_rotation_for_insert(tree rt, direction inserted_dir)
{
  direction opp = inserted_dir == LEFT ? RIGHT : LEFT;
  tree a = rt;
  tree b = rt->child[inserted_dir];

  a->child[inserted_dir] = b->child[opp];
  b->child[opp] = a;
  a->bias = BALANCED;
  b->bias = BALANCED;
  return b;
}

/**
 * @brief 部分木を2重回転（挿入用）.
 * @param[in] rt 回転する部分木の根.
 * @param[in] inserted_dir 成長した部分木の方向.
 */
tree double_rotation_for_insert(tree rt, direction inserted_dir)
{
  direction opp = inserted_dir == LEFT ? RIGHT : LEFT;
  tree a = rt;
  tree b = rt->child[inserted_dir];
  tree c = b->child[opp];

  a->child[inserted_dir] = c->child[opp];
  b->child[opp] = c->child[inserted_dir];
  c->child[inserted_dir] = b;
  c->child[opp] = a;

  if (c->bias != inserted_dir)
  {
    a->bias = BALANCED;
  }
  else
  {
    a->bias = opp;
  }

  if (c->bias != opp)
  {
    b->bias = BALANCED;
  }
  else
  {
    b->bias = inserted_dir;
  }

  c->bias = BALANCED;

  return c;
}

/**
 * @brief 部分木のバランスを条件を満たすように回転で修正（挿入用）.
 * @param[in] rt バランス状態を調べる部分木の根.
 * @param[in] inserted_dir 成長した部分木の方向.
 * @param[out] grown 部分木が成長したかどうかを反映.
 */
void rebalance_for_insert(tree *rt, direction inserted_dir, bool *grown)
{
  direction opp = inserted_dir == LEFT ? RIGHT : LEFT;
  tree a = *rt;
  if (a->bias == opp)
  {
    a->bias = BALANCED;
  }
  else if (a->bias == BALANCED)
  {
    a->bias = inserted_dir;
    *grown = true;
  }
  else
  {
    tree b = a->child[inserted_dir];
    if (b->bias == inserted_dir)
    {
      *rt = single_rotation_for_insert(a, inserted_dir);
    }
    else if (b->bias == opp)
    {
      *rt = double_rotation_for_insert(a, inserted_dir);
    }
    else
    {
      fprintf(stderr, "ERROR: Unexpected condition in rebalance_for_insert.\n");
      exit(1);
    }
  }
}

/**
 * @brief AVL木の挿入.
 * @param[in] rt 挿入対象のtreeのポインタ.
 * @param[in] np 挿入するnodeのポインタ.
 * @param[in,out] grown 部分木が成長したかどうかを反映.
 */
void insert_node(tree *rt, node *np, bool *grown)
{
  //ノードの挿入が可能な関数にgを渡し,戻って来た際に,リバランスが必要かどうか見る.
  // gは呼び出された関数内で条件式に用いる.
  // grownは呼び出した関数内で条件式に用いる/ここではinsert_nodeを呼び出した関数内.
  bool g;
  tree *p = rt;
  *grown = false;
  if (*p == NULL)
  {
    *p = np;
    *grown = true;
  }
  else if (np->rec->key < (*p)->rec->key)
  {
    insert_node(&(*rt)->child[LEFT], np, &g);
    if (g)
    {
      rebalance_for_insert(p, LEFT, grown);
    }
  }
  else if (np->rec->key > (*p)->rec->key)
  {
    insert_node(&(*p)->child[RIGHT], np, &g);
    if (g)
    {
      rebalance_for_insert(p, RIGHT, grown);
    }
  }
  else
  {
    printf("ERROR: The key is already in the tree.\n");
  }
}

/**
 * @brief 部分木を1重回転（削除用）.
 * @param[in] rt 回転する部分木の根.
 * @param[in] deleted_dir 成長した部分木の方向.
 * @param[out] shrinked 部分木が縮小したかどうかを反映.
 */
tree single_rotation_for_delete(tree rt, direction deleted_dir, bool *shrinked)
{
  direction opp = deleted_dir == LEFT ? RIGHT : LEFT;
  tree a = rt;
  tree b = a->child[opp];

  a->child[opp] = b->child[deleted_dir];
  b->child[deleted_dir] = a;
  if (b->bias == BALANCED)
  {
    a->bias = opp;
    b->bias = deleted_dir;
    *shrinked = false;
  }
  else
  {
    a->bias = BALANCED;
    b->bias = BALANCED;
    *shrinked = true;
  }
  return b;
}

/**
 * @brief 部分木を2重回転（削除用）.
 * @param[in] rt 回転する部分木の根.
 * @param[in] deleted_dir 成長した部分木の方向.
 * @param[out] shrinked 部分木が縮小したかどうかを反映.
 */
tree double_rotation_for_delete(tree rt, direction deleted_dir, bool *shrinked)
{
  direction opp = deleted_dir == LEFT ? RIGHT : LEFT;
  tree a = rt;
  tree b = a->child[opp];
  tree c = b->child[deleted_dir];

  a->child[opp] = c->child[deleted_dir];
  b->child[deleted_dir] = c->child[opp];
  c->child[deleted_dir] = a;
  c->child[opp] = b;

  if (c->bias != opp)
  {
    a->bias = BALANCED;
  }
  else
  {
    a->bias = deleted_dir;
  }

  if (c->bias != deleted_dir)
  {
    b->bias = BALANCED;
  }
  else
  {
    b->bias = opp;
  }

  c->bias = BALANCED;
  *shrinked = true;
  return c;
}

/**
 * @brief 部分木のバランスを条件を満たすように回転で修正（削除用）.
 * @param[in] rt バランス状態を調べる部分木の根.
 * @param[in] deleted_dir 縮小した部分木の方向.
 * @param[in,out] shrinked 部分木が縮小したかどうかを反映.
 */
void rebalance_for_delete(tree *rt, direction deleted_dir, bool *shrinked)
{
  direction opp = deleted_dir == LEFT ? RIGHT : LEFT;
  tree a = *rt;
  if (a->bias == BALANCED)
  {
    a->bias = opp;
    *shrinked = false;
  }
  else if (a->bias == deleted_dir)
  {
    a->bias = BALANCED;
    *shrinked = true;
  }
  else
  {
    tree b = a->child[opp];
    if (b->bias != deleted_dir)
    {
      *rt = single_rotation_for_delete(a, deleted_dir, shrinked);
    }
    else
    {
      *rt = double_rotation_for_delete(a, deleted_dir, shrinked);
    }
  }
}

/**
 * @brief 部分木内の最大nodeを取り出す.
 * @param[in] rt 最大キーを調べるtreeのポインタ.
 * @param[out] shrinked 部分木が縮小したかどうかを反映.
 * @return 最大キーをもつnodeを指すtree.
 */
tree extract_max(tree *rt, bool *shrinked)
{
  tree *p = rt;
  tree max;
  bool s;

  if ((*p)->child[RIGHT] == NULL)
  {
    max = *p;
    *p = (*p)->child[LEFT];
    *shrinked = true;
    return max;
  }
  else
  {
    max = extract_max(&(*p)->child[RIGHT], &s);
    *shrinked = false;

    if (s)
    {
      rebalance_for_delete(rt, RIGHT, shrinked);
    }
  }

  return max;
}

/**
 * @brief AVL木内のtarget==rec->keyとするnodeを削除.
 * @param[in] rt 削除対象を調べるtreeのポインタ.
 * @param[in] target 調べるrec->key.
 * @param[in,out] shrinked 部分木が縮小したかどうかを反映.
 */
void delete_node(tree *rt, size_t target, bool *shrinked)
{
  //ノードの削除が可能な関数にsを渡し,戻って来た際に,リバランスが必要かどうか見る.
  // sは呼び出された関数内で条件式に用いる.
  // shrinkedは呼び出した関数内で条件式に用いる/ここではdelete_nodeを呼び出した関数内.
  bool s;
  *shrinked = false;
  tree *p = rt;

  if (p == NULL)
  {
    printf("The key cannot be found.\n");
  }
  else if (target < (*p)->rec->key)
  {
    delete_node(&(*p)->child[LEFT], target, &s);
    if (s)
    {
      rebalance_for_delete(p, LEFT, shrinked);
    }
  }
  else if (target > (*p)->rec->key)
  {
    delete_node(&(*p)->child[RIGHT], target, &s);
    if (s)
    {
      rebalance_for_delete(p, RIGHT, shrinked);
    }
  }
  else
  {
    tree t;
    if ((*p)->child[LEFT] == NULL)
    {
      t = *p;
      *p = (*p)->child[RIGHT];
      dispose(t);
      *shrinked = true;
    }
    else
    {
      t = extract_max(&(*p)->child[LEFT], &s);
      t->child[LEFT] = (*p)->child[LEFT];
      t->child[RIGHT] = (*p)->child[RIGHT];
      t->bias = (*p)->bias;
      dispose(*p);
      *p = t;
      if (s)
      {
        rebalance_for_delete(p, LEFT, shrinked);
      }
    }
  }
}

/**
 * @brief 二分探索木を探索.
 * @param[in] rt targetが存在するか調べるtree.
 * @param[in] target 調べるキー.
 * @return 存在する場合:対応するnodeのポインタ,存在しない場合:NULL.
 */
node *search_node(tree rt, size_t target)
{
  tree p = rt;
  while (p != NULL)
  {
    if (target == p->rec->key)
    {
      return p;
    }
    else if (target < p->rec->key)
    {
      p = p->child[LEFT];
    }
    else
    {
      p = p->child[RIGHT];
    }
  }
  return NULL;
}

/**
 * @brief record確認用プリント関数.
 * @param[in] rec プリントするrecordのポインタ.
 */
void print_record(record *rec)
{
  printf("%08zu, \"%s\"\n", rec->key, rec->field);
}
/**
 * @brief node確認用プリント関数.
 * @param[in] node プリントするnodeのポインタ.
 */
void print_node(node *n)
{
  print_record(n->rec);
}

/**
 * @brief search_node確認用プリント関数.
 * @param[in] rt targetが存在するか調べるtree.
 * @param[in] target 調べるキー.
 */
void print_search_node(tree rt, size_t target)
{
  tree search_result_node = search_node(rt, target);
  printf("\"%zu\" was %s\n", target, search_result_node != NULL ? "FOUND." : "NOT FOUND.");
}

/**
 * @brief 二分探索木を再帰でプリント.再帰を展開した実装をしてみてください.
 * @param[in] p プリントする対象のtree.
 * @param[in] depth 再帰呼び出しの深さ.
 */
void print_tree_recursion(tree p, size_t depth)
{
  if (p != NULL)
  {
    print_tree_recursion(p->child[RIGHT], depth + 1);

    for (size_t i = 0; i < depth; i++)
    {
      printf(" ");
    }
    printf("+-%3zu, \"%s\"\n", p->rec->key, p->rec->field);

    print_tree_recursion(p->child[LEFT], depth + 1);
  }
}
/**
 * @brief 二分探索木をプリント.
 * @param[in] rt 操作する対象のtree.
 */
void print_tree(tree rt)
{
  size_t depth = 0;
  printf("VISUALISING TREE\n");
  printf("================================\n");
  print_tree_recursion(rt, depth);
  printf("================================\n");
}

/**
 * @brief cliからrecordの内容を読み取りrecordに格納.
 * @return cliから読み取った内容で生成したrecordのポインタ
 */
record *cli_record()
{
  record *rec;
  size_t key = -1;
  char field[MAX_FIELD_MEMORY];

  printf("Type in a key >= 0 and a field. (example: \"10001 BBB\")\n");
  printf(STRINGFY(MAX_FIELD_MEMORY) "=" DEF_STRINGFY(MAX_FIELD_MEMORY) "\n");

  while (key == (size_t)-1)
  {
    scanf("%zu", &key);
  }
  scanf("%" DEF_STRINGFY(MAX_FIELD_MEMORY) "s%*[^\n]", field);

  rec = init_record(key, field);

  return rec;
}

/**
 * @brief cliからrecordの内容を読み取り,tableの末尾に挿入.
 * @param[in] rt スキャンしたrecordを挿入するtreeのポインタ.
 */
void cli_insert(tree *rt)
{
  printf("ENETER A RECORD THAT WILL BE INSERTED.\n");
  record *scanned_rec = NULL;
  while (true)
  {
    scanned_rec = cli_record();
    tree search_result_node = search_node(*rt, scanned_rec->key);
    if (search_result_node != NULL)
    {
      printf("The key is already used.\n");
    }
    else
    {
      break;
    }
  }
  bool dummy;
  insert_node(rt, init_node(scanned_rec), &dummy);
}

int main()
{
  size_t tmp[] = {1, 3, 2};
  tree rt = NULL;
  bool dummy;

  for (size_t i = 0; i < sizeof(tmp) / sizeof(size_t); i++)
  {
    insert_node(&rt, init_node(init_record(tmp[i], "AAAA")), &dummy);
  }
  print_tree(rt);

  release_tree(&rt);
  return 0;
}