#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INF 1000000
#define MAX_SIZE 100

typedef struct {
    int prev;
    int weight;
    bool visited;
} node;

typedef struct {
    int size;
    int mat[MAX_SIZE][MAX_SIZE];
} adj_mat;

adj_mat graph;
node nodes[MAX_SIZE];

int dijkstra(int start, int goal) {
    nodes[start].weight = 0;
    while (true) {
        int min = INF;
        int target;

        for (int i = 0; i < graph.size; ++i) {
            if (!nodes[i].visited && nodes[i].weight < min) {
                target = i;
                min = nodes[i].weight;
            }
        }
        assert(min != INF);

        if (target == goal) {
            return nodes[goal].weight;
        }
        nodes[target].visited = true;

        for (int neighbor = 0; neighbor < graph.size; ++neighbor) {
            if (nodes[target].weight + graph.mat[target][neighbor] < nodes[neighbor].weight) {
                nodes[neighbor].weight = graph.mat[target][neighbor] + nodes[target].weight;
                nodes[neighbor].prev = target;
            }
        }
    }
}

void init_graph_and_nodes() {
    for (int i = 0; i < MAX_SIZE; ++i) {
        nodes[i].weight = INF;
        nodes[i].visited = false;
        nodes[i].prev = -1;
        for (int j = 0; j < MAX_SIZE; ++j)
            graph.mat[i][j] = INF;
    }
}

int main(void) {
    init_graph_and_nodes();

    graph.size = 6;
    graph.mat[0][1] = 6;
    graph.mat[0][2] = 4;
    graph.mat[1][3] = 3;
    graph.mat[2][4] = 3;
    graph.mat[2][5] = 6;
    graph.mat[4][1] = 2;
    graph.mat[4][3] = 1;
    int start = 0;
    int goal = 3;

    printf("Weight: %d\n", dijkstra(start, goal));
    int route_node = goal;
    printf("Route: %d", route_node);
    while (true) {
        route_node = nodes[route_node].prev;
        printf(" <- %d", route_node);
        if (route_node == start)
            break;
    }
    printf("\n");
    return 0;
}

// 実行結果
// Weight: 8
// Route: 3 <- 4 <- 2 <- 0
