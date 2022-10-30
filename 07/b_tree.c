#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 5

typedef enum {
    INTERNAL,
    EXTERNAL,
} node_type;

typedef struct node_ node;

typedef struct {
    node* ptr;
    int bound;
} pair;

struct node_ {
    node_type tag;

    // 各インスタンスは internal か external の一方の
    // データのみを必要とするため、無名共用体を使います。
    union {
        struct {
            int count;
            pair children[M];
        } internal;

        struct {
            int key;
            char value[32];
        } external;
    };
};

node* init_internal_node(int count) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->tag = INTERNAL;
    new_node->internal.count = count;
    return new_node;
}

node* init_external_node(int key, const char* value) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->tag = EXTERNAL;
    new_node->external.key = key;
    strcpy(new_node->external.value, value);
    return new_node;
}

int locate(node* n, int target) {
    int low = 1;
    int high = n->internal.count - 1;
    while (low <= high) {
        int middle = (low + high) / 2;
        if (target < n->internal.children[middle].bound) {
            high = middle - 1;
        } else {
            low = middle + 1;
        }
    }
    return high;
}

// target が見つかった場合はその node へのポインタを返す
// 見つからなかった場合は NULL を返す
node* search(node* root, int target) {
    if (root == NULL) {
        return NULL;
    }

    node* current = root;
    while (current->tag == INTERNAL) {
        int index = locate(current, target);
        current = current->internal.children[index].ptr;
    }
    if (current->external.key == target) {
        return current;
    }
    return NULL;
}

void swap(node** a, node** b) {
    node* tmp = *a;
    *a = *b;
    *b = tmp;
}

// TODO: insert(node, pair secondary, int pos)

bool insert_(node** p_current, int key, const char* value, pair** p_secondary) {
    node* current = *p_current;
    pair* secondary = *p_secondary;
    if (current->tag == EXTERNAL) {
        assert(current->external.key != key);

        node* new_node = init_external_node(key, value);
        if (key < current->external.key) {
            swap(p_current, &new_node);
        }

        secondary->ptr = new_node;
        secondary->bound = new_node->external.key;
        return true;
    }

    // これ以降は current は内点
    int index = locate(current, key);
    node* child = current->internal.children[index].ptr;
    bool expanded = insert_(&child, key, value, p_secondary);
    if (!expanded) {
        // このノードに対して新しい頂点を追加しなかったのであれば終了
        return false;
    }

    if (current->internal.count < M) {
        // まだ子ノードを入れられる場合
        // insert: 4
        // before: [1][3][5][ ]
        // after:  [1][3][4][5]
        //               ^^^ added

        for (int j = current->internal.count - 1; j >= index + 1; j--) {
            current->internal.children[j + 1] = current->internal.children[j];
        }
        current->internal.children[index + 1] = *secondary;
        current->internal.count++;
        return false;
    } else {
        // もう子ノードを入れられない場合
        // これ以上追加できないため、ノードを分割する。

        // 分割してノードの半数を移動するための新しい内点を作成。
        node* new_node = init_internal_node(0);

        // ノードを分割する箇所を計算。
        // M = 4 とすると split_index = 1
        int split_index = (M + 1) / 2 - 1;

        if (index >= split_index) {
            // ノードを追加したい箇所が n 以上であれば、
            // 分割して新しく作った方の内点に追加する。

            // insert 6 (index=2)
            // before: [1][3][5][7]
            // after1: [1][3][ ][ ]  [5][7][ ][ ]
            //                       ^^^^^^^^^^^^ new internal node
            // after2: [1][3][ ][ ]  [5][6][7][ ]
            //                          ^^^ added

            // split_index = 1 とすると
            // new_node[0] = current[2] // 5
            // new_node[1] = added node // 6 (index=2)
            // new_node[2] = current[3] // 7
            for (int j = split_index + 1; j <= index; j++) {
                new_node->internal.children[j - split_index - 1] = current->internal.children[j];
            }
            new_node->internal.children[index - split_index] = *secondary;
            for (int j = index + 1; j < M; j++) {
                new_node->internal.children[j - split_index] = current->internal.children[j];
            }
            current->internal.count = split_index + 1;
            new_node->internal.count = M - split_index;
        } else {  // index < n
            // ノードを追加したい箇所が n 未満であれば、
            // 分割して残った方の内点に追加する。

            // insert 2 (index=0)
            // before: [1][3][5][7]
            // after1: [1][3][ ][ ]  [5][7][ ][ ]
            //                       ^^^^^^^^^^^^ new internal node
            // after2: [1][2][3][ ]  [5][7][ ][ ]
            //            ^^^ added

            // TODO: create new_index = 0
            for (int j = split_index + 1; j < M; j++) {
                new_node->internal.children[j - split_index - 1] = current->internal.children[j];
            }
            for (int j = split_index; j >= index + 1; j--) {
                current->internal.children[j + 1] = current->internal.children[j];
            }
            current->internal.children[index + 1] = *secondary;
            current->internal.count = split_index + 2;
            new_node->internal.count = M - split_index - 1;
        }
        secondary->ptr = new_node;
        secondary->bound = new_node->internal.children[0].bound;
        return true;
    }
}

void insert(node** p_root, int key, const char* value) {
    if (*p_root == NULL) {
        *p_root = init_external_node(key, value);
        return;
    }

    pair* secondary = (pair*)malloc(sizeof(pair));
    if (insert_(p_root, key, value, &secondary)) {
        node* new_root = init_internal_node(2);
        new_root->internal.children[0].ptr = *p_root;
        new_root->internal.children[1] = *secondary;
        *p_root = new_root;
    }
}

void print(node* current, int depth) {
    if (current == NULL) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    if (current->tag == INTERNAL) {
        printf("[ ");
        for (int i = 1; i < current->internal.count; i++) {
            printf("%d ", current->internal.children[i].bound);
        }
        printf("]\n");

        for (int i = 0; i < current->internal.count; i++) {
            print(current->internal.children[i].ptr, depth + 1);
        }
    } else {
        printf("{%d, %s}\n", current->external.key, current->external.value);
    }
}

int main() {
    node* root = NULL;
    for (int i = 0; i < 15; i++) {
        insert(&root, i, "A");
    }
    print(root, 0);

    int target = 8;
    node* result = search(root, target);
    if (result) {
        printf("%d was %s\n", target, result->external.value);
    } else {
        printf("%d was not found\n", target);
    }
}

// 実行結果
