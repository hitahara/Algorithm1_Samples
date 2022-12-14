#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INF 1000000
#define MAX_SIZE 100
#define SWAP(type, a, b)   \
    {                      \
        type swap_tmp = a; \
        a = b;             \
        b = swap_tmp;      \
    }

typedef struct {
    int u, v, w;
} edge;

// DisjointSet: 素集合を表すデータ構造
//   メンバー変数:
//     int parents[MAX_SIZE];
//   メンバー関数:
//     init_parents();
//     find_root(int x);
//     unite(int x, int y);
//     same_root(int x, int y);
int parents[MAX_SIZE];

void init_parents() {
    // 最初は全てをルートとしてDisjointSetを初期化
    for (int i = 0; i < MAX_SIZE; ++i) {
        parents[i] = i;
    }
}

int find_root(int x) {
    if (parents[x] == x) {
        return x;
    }
    return parents[x] = find_root(parents[x]);  // path compression
}

void unite(int x, int y) {
    x = find_root(x);
    y = find_root(y);
    if (x == y) {  // xとyのルートが元々同じなら何もしない
        return;
    }
    parents[x] = y;
}

bool same_root(int x, int y) {
    return find_root(x) == find_root(y);
}

void down_heap(edge* edges, int target_node_index, int end_node_index) {
    // top min
    while (true) {
        int comparing_node_index = 2 * target_node_index + 1;
        if (comparing_node_index > end_node_index) {
            break;
        }
        // comparing_node_index == end_node_indexの時に右部分木は存在しない.
        if (comparing_node_index != end_node_index) {
            if (edges[comparing_node_index + 1].w < edges[comparing_node_index].w) {
                // 右部分木に
                comparing_node_index++;
            }
        }
        if (edges[target_node_index].w < edges[comparing_node_index].w) {
            break;
        }
        SWAP(edge, edges[target_node_index], edges[comparing_node_index])
        target_node_index = comparing_node_index;
    }
}

void kruskal(edge* edges, int num_edges) {
    init_parents();
    for (int i = num_edges / 2 - 1; i >= 0; --i) {
        down_heap(edges, i, num_edges - 1);
    }

    int total_weight = 0;
    int u, v;
    for (int i = num_edges - 1; i >= 0; --i) {
        edge* e = &edges[0];
        u = e->u;
        v = e->v;
        if (!same_root(u, v)) {
            printf("%d - %d : %d\n", e->u, e->v, e->w);
            total_weight += e->w;
            unite(u, v);
        }
        if (i > 0) {
            SWAP(edge, edges[0], edges[i])
            down_heap(edges, 0, i);
        }
    }

    printf("Total weight of minimum spanning tree: %d\n", total_weight);
}

int main() {
    edge edges[8] = {{0, 1, 4}, {0, 2, 4}, {1, 2, 2}, {2, 3, 3}, {2, 5, 2}, {2, 4, 4}, {3, 4, 3}, {4, 5, 3}};
    kruskal(edges, 8);
}
