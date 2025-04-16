#include "graph.h"

static inline int GetValue(int l, int c, Graph board)
{
    if(l < 0 || c < 0 || l == board.size.l || c == board.size.c) {
        return 0; //if its out of bounds then return 0 (dead cell state)
    } else {
        return board.matrix[l][c];
    }
}

void add_in_vector(int l, int c, Vector *vector)
{
    if(vector->count == vector->size){
        vector->size *= 2;
        vector->value = (Coord*) realloc(vector->value, (vector->size) * sizeof(Coord));
    }

    vector->value[vector->count].l = l;
    vector->value[vector->count].c = c;
    vector->count++;
}

void floodSave(int l, int c, int original, int new, Graph *board, Vector *Island)
{
    const char neighbours[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1},
        { 0, -1},          { 0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    };

    board->matrix[l][c] = new;

    add_in_vector(l, c, Island);


    for(int k = 0; k < 8; k++){
        int neighbour = GetValue(l + neighbours[k][0], c + neighbours[k][1], *board);

        if(neighbour == original )
            floodSave(l + neighbours[k][0], c + neighbours[k][1], original, new, board, Island);
        
        if(neighbour == original || neighbour == new)
            Island->value[Island->count - 1].degree++;
    }
}

VectorVector getIslands(Graph board)
{
    VectorVector islands = {
        .value = NULL,
        .count = 0
    };
    
    for(int i = 0; i < board.size.l; i++){
        for(int j = 0; j < board.size.c; j++){
            if(GetValue(i, j, board) == 1){
                islands.value = (Vector*) realloc(islands.value, (islands.count + 1) * sizeof(Vector));
                
                Vector* island = &islands.value[islands.count];

                island->value = (Coord*) malloc(sizeof(Coord));
                island->count = 0;
                island->size = 1;                

                floodSave(i, j, 1, 2, &board, island);
                
                islands.count++;
            }
        }
    }

    return islands;
}

int Hamiltonian(Coord start, int dimension, int total_dimension, Vector *HamiltonianChain, Graph board){
    
    if(GetValue(start.l, start.c, board) != 2)
        return 0;   //invalid coordonates

    const char neighbours[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1},
        { 0, -1},          { 0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    };
    
    HamiltonianChain->value[dimension - 1] = start;
    board.matrix[start.l][start.c] = 3; //mark visited
    
    if(dimension == total_dimension){
        board.matrix[start.l][start.c] = 2; //unmark
        return 1;   // good path
    }
    
    for(int k = 0; k < 8; k++){
        Coord next = {
            .l = start.l + neighbours[k][0],
            .c = start.c + neighbours[k][1]
        };

        if(Hamiltonian(next, dimension + 1, total_dimension, HamiltonianChain, board) == 1){    //good path
            board.matrix[start.l][start.c] = 2; //unmark
            return 1;
        }
    }

    board.matrix[start.l][start.c] = 2; //unmark
    return -1;                          //incorrect path
}

void LongestHamiltonian(FILE* output_file, Graph board)
{
    VectorVector Islands = getIslands(board);

    Vector Chain = {
        .count = 0,
        .size = Islands.value[0].count,
        .value = NULL
    };

    Vector MaxChain = {
        .count = 0,
        .size = 1,
        .value = NULL
    };

    int isValid = 0;
    
    for(int j = 0; j < Islands.count; j++){
        Vector* island = &Islands.value[j];

        if (Chain.size < island->count) {
            Chain.size = island->count;
            Chain.value = (Coord*) realloc(Chain.value, Chain.size * sizeof(Coord));
        }

        for(int i = 0; i < island->count; i++){
            Chain.count = 0;
            int valid = Hamiltonian(island->value[i], 1, island->count, &Chain, board);
            Chain.count = island->count;


            if(valid == 1){
                isValid = 1;

                if(Chain.count > MaxChain.count){
                    MaxChain.count = Chain.count;
                    MaxChain.size = Chain.count;
                    MaxChain.value = (Coord*) realloc(MaxChain.value, MaxChain.size * sizeof(Coord));
                    for(int k = 0; k < MaxChain.count; k++)
                        MaxChain.value[k] = Chain.value[k];
                }   

                break;
            }
        }
    }
    
    if(isValid){
        fprintf(output_file,"%d\n", MaxChain.count - 1);
        for(int i = 0; i < MaxChain.count; i++){
            fprintf(output_file,"(%d,%d)", MaxChain.value[i].l, MaxChain.value[i].c);
            if(i < MaxChain.count - 1)
                fprintf(output_file," ");
        }
        fprintf(output_file,"\n");
    } else {
        fprintf(output_file,"-1\n");
    }


    free(Chain.value);
    free(MaxChain.value);                       //rewrite to return the vector with the node coordonates instead of writing in the function
    for(int i = 0; i < Islands.count; i++)
        free(Islands.value[i].value);
    free(Islands.value);
}
