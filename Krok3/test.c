#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pomoc.h"

//mozna potem dodac watki

struct Node{
    int y;
    int x;
    struct Node *next;
};

void insert_node(struct Node **head, int y, int x){
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->y = y;
    new_node->x = x;
    new_node->next = *head;
    *head = new_node;
}

void delete_start(struct Node **head){
    if(*head == NULL)
        return;
    struct Node *temp = *head;
    *head = (*head)->next;
    free(temp);
}

void fill_infile_name(char *fname, int i){
    strcpy(fname, "Res/p");
    char snum[5];
    itoa(i, snum, 10);
    strcat(fname, snum);
}

void fill_outfile_name(char *fname, int i){
    getcwd(fname, PATH_MAX);
    char a = '\\';
    strncat(fname, &a, 1);
    strcat(fname, "Res2\\p");
    char snum[5];
    itoa(i, snum, 10);
    strcat(fname, snum);
    strcat(fname, "_ok.ppm");
}

void read_bitmap(int width, int height, int bitmap[height][width], FILE* finptr, struct Node **point_list){
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

bool is_contour(int width, int height, int bitmap[height][width], int x, int y){
    if(y==0 || y==height-1 || x==0 || x==width-1)
        return false;
    if(bitmap[y][x+1]==0 || bitmap[y][x-1]==0 || bitmap[y+1][x]==0 || bitmap[y-1][x]==0)
        return true;
    return false;
}

bool on_bitmap(int width, int height, int x, int y){
    return (x>=0 && x<width && y>=0 && y<height);
}

int find_contour_neighbours(int width, int height, int bitmap[height][width], int x_start, int y_start){
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
                if(on_bitmap(width, height, x+j, y+i) && bitmap[y+i][x+j]==1 && is_contour(width, height, bitmap, x+j, y+i)){
                    bitmap[y+i][x+j] = group;
                    cnt++;
                    insert_node(&queue, y+i, x+j);
                }
            }
        }
    }
    return cnt;
}

int get_puzzle_contour(int width, int height, int bitmap[height][width], struct Node *point_list){
    int next_group = 2;
    int max_group = 2;
    int max_group_size = 0;
    while(point_list != NULL){
        int y = point_list->y;
        int x = point_list->x;
        if(bitmap[y][x]>1 || (is_contour(width, height, bitmap, x, y) == false))
            point_list = point_list->next;
        else{
            bitmap[y][x] = next_group;
            int cnt = find_contour_neighbours(width, height, bitmap, x, y);
            if(cnt > max_group_size){
                max_group_size = cnt;
                max_group = next_group;
            }
            next_group++;
        }
    }
    return max_group;
}

void bitmap_to_file(int width, int height, int bitmap[height][width], FILE* foutptr, int group){
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(bitmap[y][x] == group)
                fprintf(foutptr, "%d %d %d\n", 0, 0, 0);
            else
                fprintf(foutptr, "%d %d %d\n", 255, 255, 255);
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
        read_bitmap(width, height, bitmap, finptr, &point_list);

        int contour_num = get_puzzle_contour(width, height, bitmap, point_list);
        bitmap_to_file(width, height, bitmap, foutptr, contour_num);

        fclose(finptr);
        fclose(foutptr);
    }
}
