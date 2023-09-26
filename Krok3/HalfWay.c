#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "structs.h"
#include "pomoc.h"
#include "linked_list.h"
#include "bitmap.h"
#include "contour.h"
#include "describe_side.h"
#include "corners.h"
#include "puzzle_match.h"
#include "solve_boarder.h"
#include "solve_puzzle.h"

int main(){
    int puzzle_pieces;
    printf("Podaj liczbe puzzli: ");
    scanf("%d", &puzzle_pieces);

    struct Puzzle puzzles[puzzle_pieces];

    FILE* finptr;
    char fin_name[20];
    fill_infile_name(fin_name, 1111);
    finptr = fopen(fin_name, "r");

    for(int i=0; i<puzzle_pieces; i++){
        for(int j=0; j<4; j++){
            int type, width, points_num;
            fscanf(finptr, "%d\n%d\n%d\n", &type, &width, &points_num);
            puzzles[i].sides[j].type = type;
            puzzles[i].sides[j].width = width;
            puzzles[i].sides[j].points_num = points_num;
            struct Position* positions;
            positions = (struct Position*)malloc((points_num) * sizeof(struct Position));
            puzzles[i].sides[j].positions = positions;
            for(int k=0; k<points_num; k++){
                int x, y;
                fscanf(finptr, "%d %d\n", &x, &y);
                puzzles[i].sides[j].positions[k].x = x;
                puzzles[i].sides[j].positions[k].y = y;
            }
            int heighest, lowest, left_shape_ind, right_shape_ind;
            fscanf(finptr, "%d\n%d\n%d\n%d\n", &heighest, &lowest, &left_shape_ind, &right_shape_ind);
            puzzles[i].sides[j].heighest = heighest;
            puzzles[i].sides[j].lowest = lowest;
            puzzles[i].sides[j].left_shape_ind = left_shape_ind;
            puzzles[i].sides[j].right_shape_ind = right_shape_ind;
        }
    }

    fclose(finptr);
    //

    printf("done\n");
    int obwod = 0;
    int rogow = 0;
    bool border[puzzle_pieces];
    for(int i=0; i<puzzle_pieces; i++)
        border[i] = false;
    int corners[4];

    for(int i=0; i<puzzle_pieces; i++){
        int cnt = 0;
        for(int j=0; j<4; j++){
            if(puzzles[i].sides[j].type == 0){
                border[i] = true;
                obwod++;
                cnt++;
                if(cnt == 2){
                    corners[rogow] = i;
                    rogow++;
                }
            }
        }
    }
    if(rogow != 4){
        printf("Nie udalo sie wykryc czterech rogow puzzli\n");
        return 0;
    }
    /*
    Aby obliczyc rozmiar puzzli nalezy roziwazac uklad rownan:
    xy = puzzle_pieces
    2(x+y) = obwod      =>  x = obwod/2 - y

    (obwod/2 - y)y = puzzle_pieces     =>           (-y)^2 + y(obwod/2) - puzzle_pieces = 0
    delta = obwod^2/4 -4*puzzle_pieces
    */

    double delta = (double)obwod*obwod/4 - 4*puzzle_pieces;
    double sqrt_delta = sqrt(delta);
    double y = (-1*obwod/2 - sqrt_delta)/(-2);
    double x = obwod/2 - y;
    if(ceilf(y) != y || ceilf(x) != x){
        printf("Nie udalo sie wykryc odpowiedniej liczby puzzli tworzacych ramke\n");
        return 0;
    }
    if(x < y){
        double temp = x;
        x = y;
        y = temp;
    }
    int puzzle_width = (int)x;
    int puzzle_height = (int)y;

    int puzzle_board[puzzle_height][puzzle_width];
    for(int i=0; i<puzzle_height; i++)
        for(int j=0; j<puzzle_width; j++)
            puzzle_board[i][j] = -1;

    struct BitmapInfo puzzle_solution;
    puzzle_solution.width = puzzle_width;
    puzzle_solution.height = puzzle_height;
    puzzle_solution.bitmap = (int*)puzzle_board;

    int sideUpwardBoard[puzzle_height][puzzle_width];
    for(int i=0; i<puzzle_height; i++)
        for(int j=0; j<puzzle_width; j++)
            sideUpwardBoard[i][j] = -1;

    struct BitmapInfo sideUpward;
    sideUpward.width = puzzle_width;
    sideUpward.height = puzzle_height;
    sideUpward.bitmap = (int*)sideUpwardBoard;

    struct FinalStruct finalStruct;
    finalStruct.puzzleSolution = &puzzle_solution;
    finalStruct.sideUpward = &sideUpward;
    finalStruct.puzzle_pieces = puzzle_pieces;
    finalStruct.puzzles = puzzles;

    solve_border(finalStruct, border, corners);
    solve_puzzle(&finalStruct, border);
    return 0;
}

