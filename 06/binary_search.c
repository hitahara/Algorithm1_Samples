#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_RECORDS 1000

typedef struct {
    int key;
    char value[32];
} record;

typedef struct {
    int length;
    record records[MAX_NUM_RECORDS];
} table;

// key 以上となる値が現れる最小の index を返します。
// もし条件を満たす値が存在しない場合は tab->length を返します。
// さらに補足：
//   この仕様は C++ の std::lower_bound に準拠しています。
//   講義スライドの仕様は「key 以下となる値が現れる最大の index を返す」
//   となっており、insertのコードで「table[pos+1] = data」と一つずらす
//   必要が生じています。講義上はスライドに合わせた方が分かりやすいとは
//   思うのですが、コード上分かりやすい仕様に変更させていただきました。
int search(table* tab, int target) {
    int low = 0;
    int high = tab->length - 1;
    while (low <= high) {
        int middle = (low + high) / 2;
        if (target <= tab->records[middle].key) {
            high = middle - 1;
        } else {
            low = middle + 1;
        }
    }
    return low;
}

void insert(table* tab, record rec) {
    assert(tab->length < MAX_NUM_RECORDS);

    // 順序が正しく保たれるように、正しい挿入位置を計算します
    int index = search(tab, rec.key);
    for (int i = tab->length; i > index; i--) {
        tab->records[i] = tab->records[i - 1];
    }

    tab->records[index] = rec;
    tab->length++;
}

void erase(table* tab, int pos) {
    for (int i = pos; i < tab->length - 1; i++) {
        tab->records[i] = tab->records[i + 1];
    }
    tab->length--;
}

void cli_insert(table* tab) {
    printf("Type in a key (>= 0) and a field. (example: \"100 BBB\")\n");
    while (true) {
        record rec;
        scanf("%d %s", &rec.key, rec.value);
        if (search(tab, rec.key) != tab->length) {
            printf("The key is already used.\n");
        } else {
            insert(tab, rec);
            return;
        }
    }
}

void print(table* tab) {
    printf("TABLE: [ ");
    for (int i = 0; i < tab->length; i++) {
        printf("{%d, %s} ", tab->records[i].key, tab->records[i].value);
    }
    printf("]\n");
    printf("LENGTH: %d\n\n", tab->length);
}

// 入力をシャッフルするために用意した本題とは関係ない関数です。
void shuffle(int* array, int length) {
    int i = length;
    while (i > 1) {
        int j = rand() % i--;
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

int main() {
    // create inputs
    int num_keys = 15;
    int keys[15];
    for (int i = 0; i < num_keys; i++) {
        keys[i] = i;
    }
    shuffle(keys, num_keys);

    table tab = {0};
    for (int i = 0; i < num_keys; i++) {
        record rec = {keys[i], "AAA"};
        insert(&tab, rec);
    }
    print(&tab);

    // // insert user input
    // cli_insert(&tab);
    // print(&tab);

    // search 3
    int target = 3;
    int index = search(&tab, target);
    if (index != tab.length && tab.records[index].key == target) {
        printf("%d was %s\n", target, tab.records[index].value);
    } else {
        printf("%d was NOT FOUND.\n", target);
    }

    // erase 3
    erase(&tab, index);
    print(&tab);

    // search 3
    index = search(&tab, target);
    if (index != tab.length && tab.records[index].key == target) {
        printf("%d was %s\n", target, tab.records[index].value);
    } else {
        printf("%d was NOT FOUND.\n", target);
    }

    return 0;
}

// 実行結果
// TABLE: [ {0, AAA} {1, AAA} {2, AAA} {3, AAA} {4, AAA} ]
// LENGTH: 5
//
// Type in a key (>= 0) and a field. (example: "100 BBB")
// 5 B
// TABLE: [ {0, AAA} {1, AAA} {2, AAA} {3, AAA} {4, AAA} {5, B} ]
// LENGTH: 6
//
// 3 was AAA
// TABLE: [ {0, AAA} {1, AAA} {2, AAA} {4, AAA} {5, B} ]
// LENGTH: 5
//
// 3 was NOT FOUND.
