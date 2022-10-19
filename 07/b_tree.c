#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 5

// 4 bytes
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

node* search(node* root, int target) {
    if (root == NULL) {
        return false;
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

bool insert_(node** p_current, int key, const char* value, pair** p_parent) {
    node* current = *p_current;
    if (current->tag == EXTERNAL) {
        assert(current->external.key != key);

        node* new_node = (node*)malloc(sizeof(node));
        new_node->tag = EXTERNAL;
        new_node->external.key = key;
        strcpy(new_node->external.value, value);
        if (key < current->external.key) {
            // current と new_node を swap
            node* tmp = current;
            *p_current = new_node;
            new_node = tmp;
        }

        pair* parent = *p_parent;
        parent->ptr = new_node;
        parent->bound = new_node->external.key;
        return true;
    }

    // current is INTERNAL
    // int index = locate(current, key);
    // node* child = current->internal.children[index].ptr;
    // if (insert_(&child, key, value, p_parent)) {
    //     if (current->internal.count < M) {
    //         for (int j = current->internal.count - 1; j >= index;)
    //     }
    // }
}

void insert(node* root, int key, const char* value) {
    if (root == NULL) {
        root = (node*)malloc(sizeof(node));
        root->tag = EXTERNAL;
        root->external.key = key;
        strcpy(root->external.value, value);
        return;
    }
}

int main() {
    node n;
    n.internal.count = 23;
    printf("%d\n", n.internal.count);
}

// 実行結果
