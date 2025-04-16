#include <stdio.h>
#include <stdlib.h>

/**
 * struct Coord - represents a cell coordinate in the grid.
 * @l: row index.
 * @c: column index.
 */
typedef struct Coord{
    int l;
    int c;
    int degree;
} Coord;

typedef struct Vector{
    Coord *value;
    int count;
    int size;
} Vector;

typedef struct VectorVector{
    Vector *value;
    int count;
} VectorVector;

typedef struct Graph {
    int **matrix;
    Coord size;
} Graph;

VectorVector Get_Islands(Graph board);
void LongestHamiltonian(FILE* file_output, Graph board);
//create
//free
//dfs
//bfs
//get neighbours
//size_n eulerian
