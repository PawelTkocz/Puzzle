#include <stdio.h>
#include <stdlib.h>

void find_corners(struct BitmapInfo *bitmapInfo, struct Node *point_list, int contour_len, struct Puzzle *puzzle);

void visualize_corners(struct BitmapInfo *bitmapInfo);

void select_sort_two(int n, int *tab, int *tab2);

double point_dist(int x1, int y1, int x2, int y2);

double find_angle(int x0, int y0, int x1, int y1, int x2, int y2);