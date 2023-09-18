#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pomoc.h"
#include "linked_list.h"
#include "puzzle_placement.h"
#include "bitmap.h"
#include "contour.h"
#include "corners.h"

//mozna potem dodac watki

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
        FILE *foutptr;
        foutptr = fopen(fout_name, "w");

        int width = 0;
        int height = 0;
        fscanf(finptr, "%d %d", &width, &height);
        ppm_header(foutptr, width, height);
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

        //visualize_corners(&bitmapInfo);
        //visualize_rotated_sides(&bitmapInfo, puzzles[i]);
        //visualize_sides(&bitmapInfo);
        bitmap_to_file(&bitmapInfo, foutptr);
        fclose(finptr);
        fclose(foutptr);
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
            perror("Nie udalo sie stworzyc watku\n");
    }

    for(int i=0; i<nthreads; i++)
        if(pthread_join(tid[i], NULL) != 0)
            perror("Nie udalo sie zjoinowac watku\n");

    /*
    for(int i=0; i<puzzle_pieces; i++){
        do_puzzle(puzzles, i);
    }
    */

    /*
    for(int i=0; i<puzzle_pieces; i++){
        for(int j=0; j<4; j++){
            printf("%d %lf %d\n", puzzles[i].sides[j].type, puzzles[i].sides[j].width, puzzles[i].sides[j].points_num);
            int n = puzzles[i].sides[j].points_num;
            for(int k=0; k<n; k++){
                printf("(%lf %lf)\n", puzzles[i].sides[j].positions[k].x, puzzles[i].sides[j].positions[k].y);
            }
        }
    }
    */
    /*
    int wyp = 0;
    int wkl = 0;
    int ramek = 0;
    int rogow = 0;
    for(int i=0; i<puzzle_pieces; i++){
        printf("%d\n", i);
        int cnt = 0;
        for(int j=0; j<4; j++){
            if(puzzles[i].sides[j].type == 0){
                ramek++;
                cnt++;
                if(cnt == 2)
                    rogow++;
            }
            else if(puzzles[i].sides[j].type == -1)
                wkl++;
            else
                wyp++;
        }
    }
    printf("%d %d %d %d\n", wyp, wkl, ramek, rogow);
    */
}
