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

typedef struct edgEDGE_LIST edgEDGE_LIST;
struct edgEDGE_LIST {
    int n;
    edge data[MAX_SIZE];
};

typedef struct {
    int size;
    int mat[MAX_SIZE][MAX_SIZE];
} adj_mat;

adj_mat graph;
edgEDGE_LIST EDGE_LIST;

// DisjointSet: 素集合を表すデータ構造とアルゴリズム(union-find)
// メンバー変数:
//   int parents[MAX_SIZE];
// メンバー関数:
//   int find_root(int x);
//   void unite(int x, int y);
//   bool same_root(int x, int y);
int parents[MAX_SIZE];

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

void down_heap(int target_node_index, int end_node_index) {
    // top min
    while (true) {
        int comparing_node_index = 2 * target_node_index + 1;
        if (comparing_node_index > end_node_index) {
            break;
        }
        // comparing_node_index == end_node_indexの時に右部分木は存在しない.
        if (comparing_node_index != end_node_index) {
            if (EDGE_LIST.data[comparing_node_index + 1].w < EDGE_LIST.data[comparing_node_index].w) {
                // 右部分木に
                comparing_node_index++;
            }
        }
        if (EDGE_LIST.data[target_node_index].w < EDGE_LIST.data[comparing_node_index].w) {
            break;
        }
        SWAP(edge, EDGE_LIST.data[target_node_index], EDGE_LIST.data[comparing_node_index])
        target_node_index = comparing_node_index;
    }
}

void kruskal() {
    int i, j;
    EDGE_LIST.n = 0;

    for (i = 1; i < graph.size; ++i) {
        for (j = 0; j < i; ++j) {
            if (graph.mat[i][j] != INF) {
                EDGE_LIST.data[EDGE_LIST.n].u = i;
                EDGE_LIST.data[EDGE_LIST.n].v = j;
                EDGE_LIST.data[EDGE_LIST.n].w = graph.mat[i][j];

                EDGE_LIST.n++;
            }
        }
    }

    for (int i = EDGE_LIST.n / 2 - 1; i >= 0; --i) {
        down_heap(i, EDGE_LIST.n - 1);
    }

    int total_weight = 0;

    int u, v;
    for (i = EDGE_LIST.n - 1; i >= 0; --i) {
        edge *e = &EDGE_LIST.data[0];
        u = e->u;
        v = e->v;
        if (!same_root(u, v)) {
            printf("\n %4d - %4d : %4d", e->u, e->v, e->w);
            total_weight += e->w;
            unite(u, v);
        }
        if (i > 0) {
            SWAP(edge, EDGE_LIST.data[0], EDGE_LIST.data[i])
            down_heap(0, i);
        }
    }

    printf("\nTotal weight of minimum spanning tree: %d\n", total_weight);
}

int main() {
    // 最初は全てをルートとしてDisjointSetを初期化
    for (int i = 0; i < MAX_SIZE; ++i) {
        parents[i] = i;
    }
    // グラフを初期化
    for (int i = 0; i < MAX_SIZE; ++i) {
        parents[i] = i;
        for (int j = 0; j < MAX_SIZE; ++j) {
            graph.mat[i][j] = INF;
        }
    }

    graph.size = 6;
    int abw[8][3] = {{0, 1, 4}, {0, 2, 4}, {1, 2, 2}, {2, 3, 3}, {2, 5, 2}, {2, 4, 4}, {3, 4, 3}, {4, 5, 3}};
    for (int i = 0; i < 8; ++i) {
        int a = abw[i][0];
        int b = abw[i][1];
        int w = abw[i][2];
        graph.mat[a][b] = w;
        graph.mat[b][a] = w;
    }

    kruskal();
}
