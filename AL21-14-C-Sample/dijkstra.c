#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INF 1000000000
#define SIZE 100

typedef struct node node;
struct node
{
  int via;
  int weight;
  bool visited;
};

typedef struct adj_mat adj_mat;
struct adj_mat
{
  int max_size;
  int mat[SIZE][SIZE];
};

adj_mat GRAPH;
node NODES[SIZE];

int dijkstra(int start, int goal)
{
  int min, target;

  NODES[start].weight = 0;

  while (true)
  {
    min = INF;

    for (int i = 0; i < GRAPH.max_size; ++i)
    {
      if (NODES[i].visited == false && NODES[i].weight < min)
      {
        target = i;
        min = NODES[i].weight;
      }
    }

    if (min == INF)
    {
      fprintf(stderr, "ERROR: s=%d and s=%d is not in the same graph.\n", start, goal);
      exit(1);
    }

    if (target == goal)
      return NODES[goal].weight;

    NODES[target].visited = true;

    for (int neighbor = 0; neighbor < GRAPH.max_size; ++neighbor)
    {
      if (NODES[target].weight + GRAPH.mat[target][neighbor] < NODES[neighbor].weight)
      {
        NODES[neighbor].weight = GRAPH.mat[target][neighbor] + NODES[target].weight;
        NODES[neighbor].via = target;
      }
    }
  }
}

void init_graph_and_nodes()
{
  for (int i = 0; i < SIZE; ++i)
  {
    NODES[i].weight = INF;
    NODES[i].visited = false;
    NODES[i].via = -1;
    for (int j = 0; j < SIZE; ++j)
      GRAPH.mat[i][j] = INF;
  }
}

int main(void)
{
  int edge_count;
  int a, b, w;
  int start, goal;

  init_graph_and_nodes();

  /*
  A : 0
  B : 1
  C : 2
  D : 3
  E : 4
  F : 5

  6 7
  0 1 6
  0 2 4
  1 3 3
  2 4 3
  2 5 6
  4 1 2
  4 3 1
  0 3
  */

  scanf("%d %d", &(GRAPH.max_size), &edge_count);
  for (int i = 0; i < edge_count; ++i)
  {
    scanf("%d %d %d", &a, &b, &w);
    GRAPH.mat[a][b] = w;
  }
  scanf("%d %d", &start, &goal);

  printf("Total weight: %d\n", dijkstra(start, goal));

  int route_node = goal;
  printf("%d", route_node);
  while (true)
  {
    route_node = NODES[route_node].via;
    printf(" <- %d", route_node);
    if (route_node == start)
      break;
  }

  printf("\n");
  return 0;
}
