#include <stdio.h>
#include <stdlib.h>

struct BitmapInfo{
    int width;
    int height;
    int *bitmap;
};

void set_bitmap(struct BitmapInfo *bitmapInfo, int x, int y, int val){
    int *bitmap = bitmapInfo->bitmap;
    int width = bitmapInfo->width;
    *((bitmap+y*width) + x) = val;
}

int get_bitmap(struct BitmapInfo *bitmapInfo, int x, int y){
    int *bitmap = bitmapInfo->bitmap;
    int width = bitmapInfo->width;
    return *((bitmap+y*width) + x);
}

bool on_bitmap(struct BitmapInfo *bitmapInfo, int x, int y){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    return (x>=0 && x<width && y>=0 && y<height);
}

void bitmap_to_file(struct BitmapInfo *bitmapInfo, FILE* foutptr){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            int v = get_bitmap(bitmapInfo, x, y);
            if(v == 1)
                fprintf(foutptr, "%d %d %d\n", 0, 0, 0);
            else if(v == 2)
                fprintf(foutptr, "%d %d %d\n", 255, 0, 255);
            else if(v == 3)
                fprintf(foutptr, "%d %d %d\n", 255, 255, 0);
            else if(v == 4 || v==7)
                fprintf(foutptr, "%d %d %d\n", 255, 0, 0);
            else if(v == 5 || v==8)
                fprintf(foutptr, "%d %d %d\n", 0, 255, 0);
            else if(v == 6 || v==9)
                fprintf(foutptr, "%d %d %d\n", 0, 0, 255);
            else
                fprintf(foutptr, "%d %d %d\n", 255, 255, 255);
        }
    }
}

void read_bitmap(struct BitmapInfo *bitmapInfo, FILE* finptr, struct Node **point_list){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            int r, g, b;
            fscanf(finptr, "%d %d %d", &r, &g, &b);
            if(puzzle_pixel(r, g, b)){
                set_bitmap(bitmapInfo, x, y, 1);
                insert_node(point_list, y, x);
            }
            else
                set_bitmap(bitmapInfo, x, y, 0);
        }
    }
}
