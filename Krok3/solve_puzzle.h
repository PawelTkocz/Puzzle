#include <stdio.h>
#include <stdlib.h>

void insert_puzzleNode(struct PuzzleNode **head, int v){
    struct PuzzleNode *new_node = malloc_Puzzlenode(v, *head);
    *head = new_node;
}

struct PuzzleByType{
    struct PuzzleNode *types[6];
};

void prepare_puzzleByType(struct PuzzleByType *puzzleByType){
    for(int i=0; i<6; i++)
        puzzleByType->types[i] = NULL;
}

int find_best(struct FinalStruct *finalStruct, bool *available, int x, int y, struct PuzzleByType *puzzleByType){
    //int down =
    return 1;
}

int get_type(struct Puzzle p){
    int sides[4];
    int cnt = 0;
    for(int i=0; i<4; i++){
        int type = p.sides[i].type;
        sides[i] = type;
        if(type == 1)
            cnt++;
    }
    if(cnt<2)
        return cnt;
    if(cnt>2)
        return cnt+1;
    if(sides[1] != sides[0] && sides[1] != sides[2])
        return 2;
    return 3;
}

void solve_puzzle(struct FinalStruct *finalStruct, bool *is_border){
    struct BitmapInfo *puzzle_solution = finalStruct->puzzleSolution;
    struct Puzzle *puzzles = finalStruct->puzzles;
    int puzzle_width = puzzle_solution->width;
    int puzzle_height = puzzle_solution->height;

    int border_pieces = 2*(puzzle_width+puzzle_height)-4;
    int pieces_num = puzzle_height*puzzle_width;
    int inner_pieces = pieces_num-border_pieces;
    printf("%d\n", inner_pieces);

    /*
   TYP       KRAWEDZIE      SUMA
    0       -1 -1 -1 -1      -4
    1        1 -1 -1 -1      -2
    2        1 -1  1 -1       0
    3        1  1 -1 -1       0
    4        1  1  1 -1       2
    5        1  1  1  1       4
    */

    struct PuzzleByType puzzleByType;
    prepare_puzzleByType(&puzzleByType);


    int pieces[inner_pieces];

    int cnt = 0;
    for(int i=0; i<pieces_num; i++)
        if(!is_border[i]){
            pieces[cnt] = i;
            int type = get_type(puzzles[i]);
            insert_puzzleNode(&(puzzleByType.types[type]), cnt);
            cnt++;
        }
    if(pieces[0]<-123)
        printf("nope\n");

    bool available[inner_pieces];
    for(int i=0; i<inner_pieces; i++)
        available[i] = true;

    for(int i=1; i<puzzle_width-1; i++){
        for(int j=1; j<puzzle_height-1; j++){
            int p = find_best(finalStruct, available, i, j, &puzzleByType);
            printf("%d\n", p);
        }
    }
}
