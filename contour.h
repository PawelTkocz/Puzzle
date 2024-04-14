#include <stdio.h>
#include <stdlib.h>

int get_puzzle_contour(struct BitmapInfo *bitmapInfo, struct Node *point_list);

int leave_only_contour(struct BitmapInfo *bitmapInfo, int group, struct Node **point_list);

int reduce_contour_pts(struct BitmapInfo *bitmapInfo, struct Node **point_list, int contour_len);