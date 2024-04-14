#include <stdio.h>
#include <stdlib.h>

void describe_sides(struct Puzzle *puzzle, struct CornerCandidate **corners, int contour_len, int *points_x, int *points_y);

void visualize_rotated_sides(struct BitmapInfo *bitmapInfo, struct Puzzle puzzle);

void visualize_sides(struct BitmapInfo *bitmapInfo);