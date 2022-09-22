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
 * @brief レコード：keyとfieldをもった構造体
 */
typedef struct record
{
  size_t key;                   /** size_t型のキー. key==-1or2^64-1を例外処理に用いている*/
  char field[MAX_FIELD_MEMORY]; /** データを保持するchar型の配列. */
} record;

struct node
{
  record *rec;
  tree left;  /** 左の枝のnodeのポインタ/tree. */
  tree right; /** 右の枝のnodeのポインタ/tree. */
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
  c->rec = rec;
  c->left = NULL;
  c->right = NULL;
  return c;
}

/**
 * @brief 前順操作.
 * @param[in] p 操作する対象のtree.
 * @param[in] something 操作を実装した関数.
 */
void pre_order(tree p, void (*something)(node *))
{
  if (p != NULL)
  {
    something(p);
    pre_order(p->left, something);
    pre_order(p->right, something);
  }
}

/**
 * @brief 間順操作.
 * @param[in] p 操作する対象のtree.
 * @param[in] something 操作を実装した関数.
 */
void in_order(tree p, void (*something)(node *))
{
  if (p != NULL)
  {
    in_order(p->left, something);
    something(p);
    in_order(p->right, something);
  }
}

/**
 * @brief 後順操作.
 * @param[in] p 操作する対象のtree.
 * @param[in] something 操作を実装した関数.
 */
void post_order(tree p, void (*something)(node *))
{
  if (p != NULL)
  {
    post_order(p->left, something);
    post_order(p->right, something);
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
 * @brief 二分探索木に挿入.
 * @param[in] rt 挿入対象のtreeのポインタ.
 * @param[in] np 挿入するnodeのポインタ.
 */
void insert_node(tree *rt, node *np)
{
  tree *p = rt;
  while (*p != NULL)
  {
    if (np->rec->key == (*p)->rec->key)
    {
      printf("The key is already used.\n");
      return;
    }
    else
    {
      if (np->rec->key < (*p)->rec->key)
      {
        p = &(*p)->left;
      }
      else
      {
        p = &(*p)->right;
      }
    }
  }
  *p = np;
}

/**
 * @brief 二分探索木の最大nodeを取り出す.
 * @param[in] rt 最大キーを調べるtreeのポインタ.
 * @return 最大キーをもつnodeを指すtree
 */
tree extract_max_node(tree *rt)
{
  tree *p = rt;
  for (; (*p)->right != NULL; p = &(*p)->right)
    ; //実体化していない時は木構造の内容に変化無し.

  tree max = *p;
  *p = (*p)->left; //実体化している時に木構造の内容は変化.
  return max;
}

/**
 * @brief 二分探索木内のtarget==rec->keyとするnodeを削除.
 * @param[in] rt 削除対象をもつtreeのポインタ.
 * @param[in] target 調べるrec->key.
 */
void delete_node(tree *rt, size_t target)
{
  tree *p = rt;
  while (*p != NULL)
  {
    if (target != (*p)->rec->key)
    {
      if (target < (*p)->rec->key)
      {
        p = &(*p)->left;
      }
      else
      {
        p = &(*p)->right;
      }
    }
    else
    {
      tree t;
      if ((*p)->left == NULL)
      {
        t = *p;
        *p = (*p)->right;
        dispose(t);
      }
      else
      {
        t = extract_max_node(&(*p)->left);
        t->left = (*p)->left;
        t->right = (*p)->right;
        dispose((*p));
        *p = t;
      }

      return;
    }
  }

  printf("A key that does not exist in the tree and cannot be deleted.\n");
}

/**
 * @brief 二分探索木を探索.
 * @param[in] rt targetが存在するか調べるtree.
 * @param[in] target 調べるキー.
 * @return 存在する場合:対応するnode*,存在しない場合:NULL.
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
      p = p->left;
    }
    else
    {
      p = p->right;
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
 * @brief 二分探索木を再帰でプリント.
 * @param[in] p 操作する対象のtree.
 * @param[in] depth 再帰呼び出しの深さ.
 */
void print_tree_recursion(tree p, size_t depth)
{
  if (p != NULL)
  {
    print_tree_recursion(p->right, depth + 1);

    for (size_t i = 0; i < depth; i++)
    {
      printf(" ");
    }
    printf("+-%3zu, \"%s\"\n", p->rec->key, p->rec->field);

    print_tree_recursion(p->left, depth + 1);
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

  while (key == (long unsigned int)-1)
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
  insert_node(rt, init_node(scanned_rec));
}

int main()
{
  size_t tmp[] = {44, 55, 12, 42, 14, 18, 06, 67};
  tree rt = NULL;
  for (size_t i = 0; i < sizeof(tmp) / sizeof(size_t); i++)
  {
    insert_node(&rt, init_node(init_record(tmp[i], "AAAA")));
  }
  print_tree(rt);

  // cli_insert(&rt);
  // print_tree(rt);

  print_search_node(rt, 44);
  delete_node(&rt, 44);
  print_search_node(rt, 44);

  // pre_order(rt, print_node);
  print_tree(rt);

  release_tree(&rt);
  print_tree(rt);

  return 0;
}