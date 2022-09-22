#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INF 1000000000
#define SIZE 100
#define SWAP(type, a, b) \
  {                      \
    type swap_tmp = a;   \
    a = b;               \
    b = swap_tmp;        \
  }

typedef struct edge edge;
struct edge {
  int u, v, w;
};

typedef struct edgEDGE_LIST edgEDGE_LIST;
struct edgEDGE_LIST {
  int n;
  edge data[SIZE];
};

typedef struct node node;
struct node {
  int via;
  int weight;
  bool visited;
};

typedef struct adj_mat adj_mat;
struct adj_mat {
  int max_size;
  int mat[SIZE][SIZE];
};

void kruskalAlgo();
int find_root(int x);
void unite(int x, int y);
bool sameRoot(int x, int y);

void init_graph();

void down_heap(size_t target_node_index, size_t end_node_index);

adj_mat GRAPH;
edgEDGE_LIST EDGE_LIST;

int U_FIND_TREE[SIZE];

void kruskalAlgo() {
  int i, j;
  EDGE_LIST.n = 0;

  for (i = 1; i < GRAPH.max_size; ++i) {
    for (j = 0; j < i; ++j) {
      if (GRAPH.mat[i][j] != INF) {
        EDGE_LIST.data[EDGE_LIST.n].u = i;
        EDGE_LIST.data[EDGE_LIST.n].v = j;
        EDGE_LIST.data[EDGE_LIST.n].w = GRAPH.mat[i][j];

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
    if (!sameRoot(u, v)) {
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

int find_root(int x) {
  if (U_FIND_TREE[x] == x) {
    return x;
  } else {
    // path compression
    // compress the path to the root.
    return U_FIND_TREE[x] = find_root(U_FIND_TREE[x]);
  }
}

void unite(int x, int y) {
  x = find_root(x);
  y = find_root(y);

  // x and y has same root already.
  if (x == y)
    return;

  U_FIND_TREE[x] = y;
}

bool sameRoot(int x, int y) {
  return find_root(x) == find_root(y);
}

void init_graph() {
  for (int i = 0; i < SIZE; ++i) {
    U_FIND_TREE[i] = i;

    for (int j = 0; j < SIZE; ++j) {
      GRAPH.mat[i][j] = INF;
    }
  }
}

int main() {
  /*
   6 8
   0 1 4
   0 2 4
   1 2 2
   2 3 3
   2 5 2
   2 4 4
   3 4 3
   4 5 3
  */

  init_graph();

  int edge_count;
  int a, b, w;
  scanf("%d %d", &(GRAPH.max_size), &edge_count);
  if (edge_count > SIZE) {
    fprintf(stderr, "ERROR: edge_count is too large. SIZE=%d\n", SIZE);
    exit(1);
  }
  for (int i = 0; i < edge_count; ++i) {
    scanf("%d %d %d", &a, &b, &w);
    GRAPH.mat[a][b] = w;
    GRAPH.mat[b][a] = w;
  }

  kruskalAlgo();
}

void down_heap(size_t target_node_index, size_t end_node_index) {
  // top min
  while (true) {
    size_t comparing_node_index = 2 * target_node_index + 1;
    if (comparing_node_index > end_node_index) {
      break;
    }
    // comparing_node_index == end_node_indexの時に右部分木は存在しない.
    if (comparing_node_index != end_node_index) {
      if (EDGE_LIST.data[comparing_node_index + 1].w < EDGE_LIST.data[comparing_node_index].w) {
        //右部分木に
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
