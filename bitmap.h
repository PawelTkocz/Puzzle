#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"

struct BitmapInfo{
    int width;
    int height;
    int *bitmap;
};

void set_bitmap(struct BitmapInfo *bitmapInfo, int x, int y, int val);

int get_bitmap(struct BitmapInfo *bitmapInfo, int x, int y);

bool on_bitmap(struct BitmapInfo *bitmapInfo, int x, int y);

void bitmap_to_file(struct BitmapInfo *bitmapInfo, FILE* foutptr);

void read_bitmap(struct BitmapInfo *bitmapInfo, FILE* finptr, struct Node **point_list);
