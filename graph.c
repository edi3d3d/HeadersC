#include "graph.h"

static inline int GetValue(int l, int c, Graph board)
{
    if(l < 0 || l == board.size.l || c < 0 || c == board.size.c) {
        return 0; //if its out of bounds then return 0 (dead cell state)
    } else {
        return board.matrix[l][c];
    }
}
void add_in_vector(int l, int c, Vector *vector)
{
    vector->value[vector->count].l = l;
    vector->value[vector->count].c = c;
    vector->count++;
    if(vector->count == vector->size){
        vector->size *= 2;
        vector->value = (Coord*) realloc(vector->value, (vector->size) * sizeof(Coord));
    }
}

void Fill(int l, int c, int original, int new, Graph board, Vector *Island)
{
    const char neighbours[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1},
        { 0, -1},          { 0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    };


    board.matrix[l][c] = new;
    int aux_count = Island->count;
    Island->value[aux_count].l=l;
    Island->value[aux_count].c=c;
    Island->count++;

    if(Island->count == Island->size){
        Island->size *= 2;
        Island->value = (Coord*) realloc(Island->value, Island->size * sizeof(Coord));
    }


    int degrees = 0;
    for(int k = 0; k < 8; k++){
        int neighbour = GetValue(l + neighbours[k][0], c + neighbours[k][1], board);
        if(neighbour == original ){
            Fill(l + neighbours[k][0], c + neighbours[k][1], original, new, board, Island);
        }
        if(neighbour == original || neighbour == new)
            degrees++;
    }
    Island->value[aux_count].degree = degrees;
}

VectorVector Get_Islands(Graph board)
{
    VectorVector Islands = {
        .value = (Vector*) malloc(sizeof(Vector)),
        .count = 0
    };
    
    for(int i = 0; i < board.size.l; i++)
        for(int j = 0; j < board.size.c; j++)
            if(GetValue(i, j, board) == 1){

                Islands.value[Islands.count].value = (Coord*) malloc(sizeof(Coord));
                Islands.value[Islands.count].count = 0;
                Islands.value[Islands.count].size = 1;
                

                Fill(i, j, 1, 2, board, &Islands.value[Islands.count]);
                

                Islands.count++;
                Islands.value = (Vector*) realloc(Islands.value, (Islands.count + 1) * sizeof(Vector));
            }
    return Islands;
}
int Hamiltonian(Coord start, int dimension, int total_dimension, Vector HamiltonianChain, Graph board){
    const char neighbours[8][2] = {
        {-1, -1}, {-1, 0}, {-1, +1},
        { 0, -1},          { 0, +1},
        {+1, -1}, {+1, 0}, {+1, +1}
    };
    
    HamiltonianChain.value[dimension - 1] = start;
    board.matrix[start.l][start.c] = 3; //from 2 (filled) to 3 (visited)
    
    if(dimension == total_dimension){
        
        board.matrix[start.l][start.c] = 2;
        return 1;
    }
    
    for(int k = 0; k < 8; k++){
        Coord next = {
            .l = start.l + neighbours[k][0],
            .c = start.c + neighbours[k][1]
        };
        if(GetValue(next.l, next.c, board) == 2){
            int aux = Hamiltonian(next, dimension + 1, total_dimension, HamiltonianChain, board);
            if(aux == 1){
                board.matrix[start.l][start.c] = 2;
                return aux;
            }
        }
    }
    board.matrix[start.l][start.c] = 2;
    HamiltonianChain.value[dimension - 1].l = -1;
    HamiltonianChain.value[dimension - 1].c = -1;
    return -1;
}

void LongestHamiltonian(FILE* output_file, Graph board)
{
    
    VectorVector Islands = Get_Islands(board);

    Vector Chain = {
        .count = 0,
        .size = Islands.value[0].count,
        .value = (Coord*) malloc(Islands.value[0].count * sizeof(Coord))
    };

    Vector MaxChain = {
        .value = (Coord*) malloc(sizeof(Coord))
    };

    int valid = -1, validMax = -1;

    for(int j = 0; j < Islands.count; j++){             //loop over every island
        valid = -1;
        for(int i = 0; i < Islands.value[j].count; i++){        //loop over each cell until a chain is found then break
            if (Chain.size < Islands.value[j].count) {
                Chain.size = Islands.value[j].count;
                Chain.value = (Coord*) realloc(Chain.value, Chain.size * sizeof(Coord));
            }

            valid = Hamiltonian(Islands.value[j].value[i], 1, Islands.value[j].count, Chain, board);
            Chain.count=Islands.value[j].count;
            if(valid == 1){
                validMax = 1;
                break;
            }
        }
        if(Chain.count > MaxChain.count && valid == 1){     //check if the new chain is a valid chain and if its longer then the already biggest found chain
            MaxChain.count = Chain.count;
            MaxChain.size = Chain.count;
            free(MaxChain.value);
            MaxChain.value = (Coord*) malloc(Chain.count * sizeof(Coord));
            for(int i = 0; i < MaxChain.count; i++)
                MaxChain.value[i] = Chain.value[i];
        }
    }
    
    if(validMax == 1){
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

    //free all except MaxChain

    free(Chain.value);

    for(int i = 0; i < Islands.count; i++) {
        free(Islands.value[i].value);
    }
    free(Islands.value);
}
