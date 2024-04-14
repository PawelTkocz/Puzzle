#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <linux/limits.h>
#include <math.h>
#include "structs.h"
#include "puzzle_processing.h"
#include "linked_list.h"
#include "bitmap.h"
#include "contour.h"
#include "describe_side.h"
#include "corners.h"
#include "puzzle_match.h"
#include "solve_border.h"
//#include "solve_inside.h"

struct ThreadStruct{
    int puzzle_pieces;
    int mod;
    int nthreads;
    struct Puzzle *puzzles;
};

void do_puzzle(struct Puzzle *puzzles, int i){
    FILE* finptr;
    char fin_name[20];
    fill_infile_name(fin_name, i);
    finptr = fopen(fin_name, "r");

    char fout_name[PATH_MAX];
    fill_outfile_name(fout_name, i);
    //FILE *foutptr;
    //foutptr = fopen(fout_name, "w");
    int width = 0;
    int height = 0;
    fscanf(finptr, "%d %d", &width, &height);
    //ppm_header(foutptr, width, height);
    int bitmap[height][width];
    struct Node *point_list = NULL;
    struct BitmapInfo bitmapInfo;
    bitmapInfo.width = width;
    bitmapInfo.height = height;
    bitmapInfo.bitmap = (int*)bitmap;
    read_bitmap(&bitmapInfo, finptr, &point_list);
    int contour_num = get_puzzle_contour(&bitmapInfo, point_list);
    int contour_len = leave_only_contour(&bitmapInfo, contour_num, &point_list);
    contour_len = reduce_contour_pts(&bitmapInfo, &point_list, contour_len);

    find_corners(&bitmapInfo, point_list, contour_len, &puzzles[i]);
    //visualize_rotated_sides(&bitmapInfo, puzzles[i]);
    //visualize_corners(&bitmapInfo);
    //visualize_sides(&bitmapInfo);
    //bitmap_to_file(&bitmapInfo, foutptr);
    fclose(finptr);
    //fclose(foutptr);
    free_list(point_list);
    printf("%d\n", i);
}

void* start_thread(void *arg){
    struct ThreadStruct *thread_struct = (struct ThreadStruct*)arg;
    int mod = thread_struct->mod;
    int puzzle_pieces = thread_struct->puzzle_pieces;
    int nthreads = thread_struct->nthreads;
    struct Puzzle *puzzles = thread_struct->puzzles;
    for(int i=mod; i<puzzle_pieces; i += nthreads){
        do_puzzle(puzzles, i);
    }
    free(arg);
    return NULL;
}

int main(){
    int puzzle_pieces;
    printf("Podaj liczbe puzzli: ");
    scanf("%d", &puzzle_pieces);
    printf("Przetwarzanie puzzli:\n");

    struct Puzzle puzzles[puzzle_pieces];

    
    int nthreads = 4;
    pthread_t tid[nthreads];
    for(int i=0; i<nthreads; i++){
        struct ThreadStruct *thread_struct = (struct ThreadStruct*)malloc(sizeof(struct ThreadStruct));
        thread_struct->mod = i;
        thread_struct->puzzles = puzzles;
        thread_struct->puzzle_pieces = puzzle_pieces;
        thread_struct->nthreads = nthreads;
        if(pthread_create(&tid[i], NULL, start_thread, thread_struct) != 0)
            printf("Nie udalo sie stworzyc watku\n");
    }
    for(int i=0; i<nthreads; i++)
        if(pthread_join(tid[i], NULL) != 0)
            printf("Nie udalo sie zjoinowac watku\n");
    
    
    /*
    //wersja nie korzystająca z wątków
    for(int i=0; i<puzzle_pieces; i++){
        do_puzzle(puzzles, i);
    }
    */

    printf("Ukończono prztwarzanie puzzli\n");
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

    printf("Program próbuje ułożyć ramkę (może to potrwać około minuty)\n");
    solve_border(finalStruct, border, corners);
    //solve_inside(&finalStruct, border);
    return 0;
}
