#include <stdio.h>
#include <stdlib.h>

//opisac krawedzie zgodnie z ruchem wskazowek zegara a nie raz w jedna raz w druga

bool get_width_points(int width1, int width2, double *res){
    int longer = width1;
    int shorter = width2;
    if(width2 > width1){
        longer = width2;
        shorter = width1;
    }

    double margin = 0.2*longer;
    int diff = longer-shorter;

    if(diff > margin)
        return true;

    double alfa = 50.0;
    *res = alfa*diff/margin;
    return false;
}

bool get_shape_points(struct PuzzleSide s1, struct PuzzleSide s2, double *res){
    struct Position *positions1 = s1.positions;
    struct Position *positions2 = s2.positions;
    int left_shape_x1 = positions1[s1.left_shape_ind].x;
    int left_shape_x2 = positions2[s2.left_shape_ind].x;
    int right_shape_x1 = positions1[s1.right_shape_ind].x;
    int right_shape_x2 = positions2[s2.right_shape_ind].x;

    int diff_left = abs(left_shape_x1-left_shape_x2);
    int diff_right = abs(right_shape_x1-right_shape_x2);

    int longer = s1.width;
    if(s1.width < s2.width)
        longer = s2.width;

    double margin = 0.2*longer;

    if(diff_left > margin && diff_right > margin)
        return true;

    double alfa = 50.0;
    *res = alfa*(diff_left+diff_right)/margin;
    return false;
}

double get_height_points(struct PuzzleSide s1, struct PuzzleSide s2){
    int height1 = s1.heighest - s1.lowest + 1;
    int height2 = s2.heighest - s2.lowest + 1;

    int higher = height1;
    if(height2 > height1){
        higher = height2;
    }

    int diff = abs(height1-height2);
    double alfa = 50.0;
    int margin = 0.2*higher;
    if(diff > margin)
        return alfa;
    else
        return alfa*diff/margin;
}

double puzzle_match_val(struct PuzzleSide s1, struct PuzzleSide s2){
    if(s1.type == 0 || s2.type == 0)
        return 100000;
    if(s1.type + s2.type != 0)
        return 100000;

    double width_points;
    bool no_match = get_width_points(s1.width, s2.width, &width_points);
    if(no_match)
        return 100000;

    double shape_points;
    no_match = get_shape_points(s1, s2, &shape_points);
    if(no_match)
        return 100000;

    double height_points = get_height_points(s1, s2);

    return width_points + shape_points + height_points;
}
