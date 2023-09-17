#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pomoc.h"
#include "linked_list.h"
#include "puzzle_placement.h"
#include "bitmap.h"
#include "contour.h"
#include "corners.h"

//mozna potem dodac watki

int main(){
    int puzzle_pieces;
    printf("Podaj liczbe puzzli: ");
    scanf("%d", &puzzle_pieces);
    //kazdy puzzel

    int contour_length[puzzle_pieces];
    int* points_x[puzzle_pieces];
    int* points_y[puzzle_pieces];
    int* corners[puzzle_pieces];
    struct PuzzleSide* sides[puzzle_pieces];

    for(int i=0; i<puzzle_pieces; i++){
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

        int *crnrs;
        struct PuzzleSide *puzzle_sides;
        crnrs = (int*)malloc(4*sizeof(int));
        puzzle_sides = (struct PuzzleSide*)malloc(4*sizeof(struct PuzzleSide));
        find_corners(&bitmapInfo, point_list, contour_len, crnrs, puzzle_sides);
        corners[i] = crnrs;
        sides[i] = puzzle_sides;
        contour_length[i] = contour_len;
        int *pts_x;
        pts_x = (int*)malloc(contour_len*sizeof(int));
        int *pts_y;
        pts_y = (int*)malloc(contour_len*sizeof(int));
        int j=0;
        struct Node* n = point_list;
        while(n != NULL){
            pts_x[j] = n->x;
            pts_y[j] = n->y;
            n = n->next;
            j++;
        }
        points_x[i] = pts_x;
        points_y[i] = pts_y;

        visualize_sides(&bitmapInfo, puzzle_sides, pts_x, pts_y, contour_len, crnrs);
        visualize_corners(&bitmapInfo);
        bitmap_to_file(&bitmapInfo, foutptr);
        fclose(finptr);
        fclose(foutptr);
        free_list(point_list);
        printf("%d\n", i);
    }

    int wyp = 0;
    int wkl = 0;
    int ramek = 0;
    int rogow = 0;
    for(int i=0; i<puzzle_pieces; i++){
        printf("%d\n", i);
        int n = contour_length[i];
        int cnt = 0;
        for(int j=0; j<4; j++){
            if(corners[i][j] < 0)
                printf("%d ", corners[i][j]);
            if(sides[i][j].type == 0){
                ramek++;
                cnt++;
                if(cnt == 2)
                    rogow++;
            }
            else if(sides[i][j].type == -1)
                wkl++;
            else
                wyp++;
        }
        for(int j=0; j<n; j++){
            if(points_x[i][j] < 0 || points_y[i][j] < 0)
                printf("(%d %d) ", points_x[i][j], points_y[i][j]);
        }
    }
    printf("%d %d %d %d\n", wyp, wkl, ramek, rogow);
}
