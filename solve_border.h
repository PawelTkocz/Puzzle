#include <stdio.h>
#include <stdlib.h>

struct FinalStruct{
    struct BitmapInfo *puzzleSolution;
    struct BitmapInfo *sideUpward;
    int puzzle_pieces;
    struct Puzzle *puzzles;
};

struct PuzzleNode{
    int ind;
    struct PuzzleNode *next;
};

struct BorderPath{
    int len;
    struct PuzzleNode *first_puzzle;
    struct PuzzleNode *last_puzzle;
    struct BorderPath *prev;
    struct BorderPath *next;
};

struct PuzzleNode* malloc_Puzzlenode(int ind, struct PuzzleNode *next);

void solve_border(struct FinalStruct finalStruct, bool *is_border, int *corners);
