#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pomoc.h"
#include "linked_list.h"

//mozna potem dodac watki

struct BitmapInfo{
    int width;
    int height;
    int **bitmap;
};

void read_bitmap(struct BitmapInfo *bitmapInfo, FILE* finptr, struct Node **point_list){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    int **bitmap = bitmapInfo->bitmap;

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            int r, g, b;
            fscanf(finptr, "%d %d %d", &r, &g, &b);
            if(puzzle_pixel(r, g, b)){
                bitmap[y][x] = 1;
                insert_node(point_list, y, x);
            }
            else
                bitmap[y][x] = 0;
        }
    }
}

bool is_contour(struct BitmapInfo *bitmapInfo, int x, int y){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    int **bitmap = bitmapInfo->bitmap;
    if(y==0 || y==height-1 || x==0 || x==width-1)
        return false;
    if(bitmap[y][x+1]==0 || bitmap[y][x-1]==0 || bitmap[y+1][x]==0 || bitmap[y-1][x]==0)
        return true;
    return false;
}

bool on_bitmap(struct BitmapInfo *bitmapInfo, int x, int y){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    return (x>=0 && x<width && y>=0 && y<height);
}

int find_contour_neighbours(struct BitmapInfo *bitmapInfo, int x_start, int y_start){
    int **bitmap = bitmapInfo->bitmap;
    int group = bitmap[y_start][x_start];
    int cnt = 1;

    struct Node *queue = NULL;
    insert_node(&queue, y_start, x_start);
    while(queue != NULL){
        int y = queue->y;
        int x = queue->x;
        delete_start(&queue);
        for(int i=-1; i<2; i++){
            for(int j=-1; j<2; j++){
                if(i==0 && j==0)
                    continue;
                if(on_bitmap(bitmapInfo, x+j, y+i) && bitmap[y+i][x+j]==1 && is_contour(bitmapInfo, x+j, y+i)){
                    bitmap[y+i][x+j] = group;
                    cnt++;
                    insert_node(&queue, y+i, x+j);
                }
            }
        }
    }
    return cnt;
}

int get_puzzle_contour(struct BitmapInfo *bitmapInfo, struct Node *point_list){
    int **bitmap = bitmapInfo->bitmap;
    int next_group = 2;
    int max_group = 2;
    int max_group_size = 0;
    while(point_list != NULL){
        int y = point_list->y;
        int x = point_list->x;
        if(bitmap[y][x]==1 && is_contour(bitmapInfo, x, y)){
            bitmap[y][x] = next_group;
            int cnt = find_contour_neighbours(bitmapInfo, x, y);
            if(cnt > max_group_size){
                max_group_size = cnt;
                max_group = next_group;
            }
            next_group++;
        }
        point_list = point_list->next;
    }
    return max_group;
}

int leave_only_contour(struct BitmapInfo *bitmapInfo, int group, struct Node *point_list){
    int **bitmap = bitmapInfo->bitmap;
    struct Node *prev = NULL;
    int len = 0;
    while(point_list != NULL){
        int y = point_list->y;
        int x = point_list->x;
        if(bitmap[y][x] == group){
            bitmap[y][x] = 1;
            prev = point_list;
            point_list = point_list->next;
            len++;
        }
        else{
            bitmap[y][x] = 0;
            if(prev == NULL){
                delete_start(&point_list);
            }
            else{
                delete_next(prev);
                point_list = prev->next;
            }
        }
    }
    return len;
}

void bitmap_to_file(struct BitmapInfo *bitmapInfo, FILE* foutptr){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    int **bitmap = bitmapInfo->bitmap;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(bitmap[y][x] == 1)
                fprintf(foutptr, "%d %d %d\n", 0, 0, 0);
            else
                fprintf(foutptr, "%d %d %d\n", 255, 255, 255);
        }
    }
}

void mark_as_available(struct BitmapInfo *bitmapInfo, int x, int y, int group){
    int **bitmap = bitmapInfo->bitmap;
    for(int i=-1; i<2; i++){
        for(int j=-1; j<2; j++){
            if(i==0 && j==0)
                continue;
            if(on_bitmap(bitmapInfo, x+j, y+i) && bitmap[y+i][x+j]==group){
                bitmap[y+i][x+j] = 1;
            }
        }
    }
}

//bool lookup_neighbours(, int x, int y, )

void reduce_contour_pts(struct BitmapInfo *bitmapInfo, struct Node *point_list, int contour_len){

    int **bitmap = bitmapInfo->bitmap;

    int start_x = point_list->x;
    int start_y = point_list->y;
    struct Node *pts_queue;
    insert_node(&pts_queue, start_y, start_x);
    bool forward = true;
    int prev_x = 0;
    int prev_y = 0;
    int cur_len = 2;

    //kiedy koniec kiedy od nowa
    while(true){
        int cur_x = pts_queue->x;
        int cur_y = pts_queue->y;
        if(forward){
            bool found = false;
            //wszystkie wolne piksele s¹siady oznaczam na bitmapie przez cur_len
            //wrzucam pierwszego wolnego piksela s¹siada do pts_queue
            for(int i=-1; i<2; i++){
                for(int j=-1; j<2; j++){
                    if(i==0 && j==0)
                        continue;
                    if(on_bitmap(bitmapInfo, cur_x+j, cur_y+i) && bitmap[cur_y+i][cur_x+j]==1){
                        if(!found){
                            found = true;
                            insert_node(&pts_queue, cur_y+i, cur_x+j);
                        }
                        bitmap[cur_y+i][cur_x+j] = cur_len;
                    }
                }
            }
            //jesli nie znalazlem zadnego wolnego piksela
            if(!found){
                forward = false;
                delete_start(&pts_queue);
                prev_x = cur_x;
                prev_y = cur_y;
                cur_len--;
            }
            else
                cur_len++;
        }
        else{
            bool found = false;
            //wszystkie piksele s¹siady s¹ ju¿ oznaczone na bitmapie przez cur_len
            //poprzez ustawienie wartoœci i oraz j w ten sposob, przeszukuje wszystkie
            //nieprzeanalizowane jeszcze piksele s¹siady. Jeœli taki siê znajdzie, dodaje
            //go do pts_queue i zwiekszam cur_len.
            for(int i=prev_y-cur_y; i<2; i++){
                for(int j=prev_x-cur_x+1; j<2; j++){
                    if(i==0 && j==0)
                        continue;
                    if(on_bitmap(bitmapInfo, cur_x+j, cur_y+i) && bitmap[cur_y+i][cur_x+j]==cur_len){
                        if(!found){
                            found = true;
                            insert_node(&pts_queue, cur_y+i, cur_x+j);
                            cur_len++;
                        }
                    }
                }
            }
            if(!found){
                delete_start(&pts_queue);
                prev_x = cur_x;
                prev_y = cur_y;
                mark_as_available(bitmapInfo, cur_x, cur_y, cur_len);
            }
        }
    }

}

int main(){
    int puzzle_pieces;
    printf("Podaj liczbe puzzli: ");
    scanf("%d", &puzzle_pieces);

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
        bitmapInfo.bitmap = (int**)bitmap;
        read_bitmap(&bitmapInfo, finptr, &point_list);

        int contour_num = get_puzzle_contour(&bitmapInfo, point_list);
        int contour_len = leave_only_contour(&bitmapInfo, contour_num, point_list);
        reduce_contour_pts(&bitmapInfo, point_list, contour_len);



        bitmap_to_file(&bitmapInfo, foutptr);
        fclose(finptr);
        fclose(foutptr);
    }
}
