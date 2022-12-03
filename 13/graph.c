#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_GRAPH_SIZE 100

typedef struct edge_cell edge_cell;
typedef struct adjacency_matrix adj_mat;
typedef struct adjacency_list adj_list;
void dfs_mat(adj_mat *mat);
void visit_mat(adj_mat *mat, bool *visit_list, int v);
void dfs_list(adj_list *list);
void visit_lst(adj_list *list, bool *visit_list, int v);

struct adjacency_matrix {
    bool M[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE];
};

struct edge_cell {
    int destination;
    edge_cell *next;
};

struct adjacency_list {
    edge_cell *adj_list[MAX_GRAPH_SIZE];
};

edge_cell *init_edge_cell(int dest) {
    edge_cell *c = (edge_cell *)malloc(sizeof(edge_cell));
    c->destination = dest;
    c->next = NULL;
    return c;
}

void dfs_mat(adj_mat *mat) {
    bool visited[MAX_GRAPH_SIZE] = {0};
    for (int i = 0; i < MAX_GRAPH_SIZE; ++i) {
        if (visited[i] == false) {
            visit_mat(mat, visited, i);
        }
    }
    printf("\n");
}

void visit_mat(adj_mat *mat, bool *visited, int v) {
    visited[v] = true;

    for (int z = 0; z < MAX_GRAPH_SIZE; ++z) {
        if (mat->M[v][z] && !visited[z]) {
            printf("%d - %d\n", v, z);
            visit_mat(mat, visited, z);
        }
    }
}

void dfs_list(adj_list *list) {
    bool visit_list[MAX_GRAPH_SIZE];

    for (int i = 0; i < MAX_GRAPH_SIZE; ++i) {
        visit_list[i] = false;
    }
    for (int i = 0; i < MAX_GRAPH_SIZE; ++i) {
        if (visit_list[i] == false) {
            visit_lst(list, visit_list, i);
        }
    }
}

void visit_lst(adj_list *list, bool *visit_list, int v) {
    visit_list[v] = true;

    edge_cell *p = list->adj_list[v];
    while (p != NULL) {
        int z = p->destination;

        if (visit_list[z] == false) {
            printf("%d - %d\n", v, z);
            visit_lst(list, visit_list, z);
        }
        p = p->next;
    }
}

int main() {
    adj_mat mat;
    for (int i = 0; i < MAX_GRAPH_SIZE; ++i) {
        for (int j = 0; j < MAX_GRAPH_SIZE; ++j) {
            mat.M[i][j] = false;
        }
    }
    mat.M[0][1] = true;
    mat.M[0][2] = true;
    mat.M[0][3] = true;
    mat.M[1][0] = true;
    mat.M[1][3] = true;
    mat.M[2][0] = true;
    mat.M[2][4] = true;
    mat.M[2][5] = true;
    mat.M[3][0] = true;
    mat.M[3][1] = true;
    mat.M[3][5] = true;
    mat.M[4][2] = true;
    mat.M[4][5] = true;
    mat.M[4][6] = true;
    mat.M[5][2] = true;
    mat.M[5][3] = true;
    mat.M[5][4] = true;
    mat.M[6][4] = true;
    dfs_mat(&mat);

    adj_list *l = (adj_list *)malloc(sizeof(adj_list));
    for (int i = 0; i < MAX_GRAPH_SIZE; ++i) {
        l->adj_list[i] = NULL;
    }
    l->adj_list[0] = init_edge_cell(2);
    l->adj_list[0]->next = init_edge_cell(3);
    l->adj_list[0]->next->next = init_edge_cell(1);
    l->adj_list[1] = init_edge_cell(0);
    l->adj_list[1]->next = init_edge_cell(3);
    l->adj_list[2] = init_edge_cell(4);
    l->adj_list[2]->next = init_edge_cell(5);
    l->adj_list[2]->next->next = init_edge_cell(0);
    l->adj_list[3] = init_edge_cell(5);
    l->adj_list[3]->next = init_edge_cell(0);
    l->adj_list[3]->next->next = init_edge_cell(1);
    l->adj_list[4] = init_edge_cell(6);
    l->adj_list[4]->next = init_edge_cell(2);
    l->adj_list[4]->next->next = init_edge_cell(5);
    l->adj_list[5] = init_edge_cell(4);
    l->adj_list[5]->next = init_edge_cell(2);
    l->adj_list[5]->next->next = init_edge_cell(3);
    l->adj_list[6] = init_edge_cell(4);
    dfs_list(l);

    return 0;
}
