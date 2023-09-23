#include <stdio.h>
#include <stdlib.h>

void describe_side_heights(struct PuzzleSide *puzzle_side){
    int n = puzzle_side->points_num;
    int highest_y = 0;
    int lowest_y = 0;
    int highest_x = 0;
    int lowest_x = 0;
    for(int i=0; i<n; i++){
        int y = puzzle_side->positions[i].y;
        if(y > highest_y)
            highest_y = y;
        if(y < lowest_y)
            lowest_y = y;
        int x = puzzle_side->positions[i].x;
        if(x > highest_x)
            highest_x = x;
        if(x < lowest_x)
            lowest_x = x;
    }
    puzzle_side->heighest = highest_y;
    puzzle_side->lowest = lowest_y;
    puzzle_side->width = highest_x - lowest_x + 1;
}

void pivot_left(int pivot_x, int pivot_y, int x, int y, double angle, double *nx, double *ny){
    double angle_rad = angle * 3.141592654 / 180;
    *nx = (x-pivot_x)*cos(angle_rad) - (y-pivot_y)*sin(angle_rad) + pivot_x;
    *ny = (x-pivot_x)*sin(angle_rad) + (y-pivot_y)*cos(angle_rad) + pivot_y;
}

void describe_shape(struct PuzzleSide *puzzle_side){
    if(puzzle_side->type == 0)
        return;

    int n = puzzle_side->points_num;
    int left_shape_x = puzzle_side->width;
    int left_ind = -1;
    int right_shape_x = 0;
    int right_ind = -1;
    int margin = 25;
    //pytanie czy nie pomyli sie left shape x z right shape x
    for(int i=0; i<n; i++){
        int x = puzzle_side->positions[i].x;
        int y = puzzle_side->positions[i].y;
        if(y < margin)
            continue;
        if(x < left_shape_x){
            left_shape_x = x;
            left_ind = i;
        }
        if(x > right_shape_x){
            right_shape_x = x;
            right_ind = i;
        }
    }
    puzzle_side->left_shape_ind = left_ind;
    puzzle_side->right_shape_ind = right_ind;
}

int describe_type(struct Puzzle *puzzle, struct CornerCandidate **corners, int i, int contour_len, int *points_x, int *points_y){
    struct CornerCandidate *c1 = corners[i];
    struct CornerCandidate *c2 = corners[(i+1)%4];
    struct CornerCandidate *c3 = corners[(i+2)%4];

    struct CornerCandidate *lower = c1;
    struct CornerCandidate *upper = c2;
    if(c2->y < c1->y){
        lower = c2;
        upper = c1;
    }
    double angle = 90.0;
    if(lower->x != upper->x)
        angle = find_angle(lower->x, lower->y, upper->x, lower->y, upper->x, upper->y);
    if(upper->x > lower->x)
        angle = 180.0 - angle;

    int margin = 25;
    int upper_line = lower->y + margin;
    int lower_line = lower->y - margin;
    int lower_cnt = 0;
    int upper_cnt = 0;
    int n = (c2->ind - c1->ind + 1 + contour_len)%contour_len;
    puzzle->sides[i].points_num = n;

    for(int j=0; j<n-1; j++){
        int x = points_x[(c1->ind + j)%contour_len];
        int y = points_y[(c1->ind + j)%contour_len];
        double nx, ny;
        pivot_left(lower->x, lower->y, x, y, angle, &nx, &ny);
        if(ny > upper_line)
            upper_cnt++;
        else if(ny < lower_line)
            lower_cnt++;
    }

    //printf("Upper cnt %d Lower cnt %d\n", upper_cnt, lower_cnt);
    int puzzle_up = 1;
    double nx, ny;
    pivot_left(lower->x, lower->y, c3->x, c3->y, angle, &nx, &ny);
    if(ny < lower->y)
        puzzle_up = -1;
    double side_len = puzzle->sides[i].width;
    int minimum = side_len/5;
    int type = 0;
    if(lower_cnt > minimum)
        type = puzzle_up;
    else if(upper_cnt > minimum)
        type = -1*puzzle_up;
    puzzle->sides[i].type = type;
    return puzzle_up;
    //lepiej oddzielna funkcja get_puzzle_position()
}

void describe_side_contour(struct Puzzle *puzzle, struct CornerCandidate **corners, int i, int contour_len, int *points_x, int *points_y, int puzzle_up){
    struct CornerCandidate *c1 = corners[i];
    struct CornerCandidate *c2 = corners[(i+1)%4];

    struct CornerCandidate *lower = c1;
    struct CornerCandidate *upper = c2;
    if(c2->y < c1->y){
        lower = c2;
        upper = c1;
    }
    double angle = 90.0;
    if(lower->x != upper->x)
        angle = find_angle(lower->x, lower->y, upper->x, lower->y, upper->x, upper->y);
    if(upper->x > lower->x)
        angle = 180.0 - angle;

    int type = puzzle->sides[i].type;


//
    double offset_x;
    double offset_y;
    pivot_left(lower->x, lower->y, points_x[(upper->ind)], points_y[(upper->ind)], angle, &offset_x, &offset_y);

    if((puzzle_up == 1 && type==1) || (puzzle_up == -1 && type==-1)){
        angle+= 180;
        offset_x = lower->x;
        offset_y = lower->y;
    }
    int n = puzzle->sides[i].points_num;

    int x = points_x[(c1->ind)%contour_len];
    int y = points_y[(c1->ind)%contour_len];
    double nx, ny;
    pivot_left(lower->x, lower->y, x, y, angle, &nx, &ny);

    struct Node *contour = NULL;
    insert_node(&contour, round(ny-offset_y), round(nx-offset_x));
    int points_added = 0;
    int prev_x = round(nx-offset_x);
    int prev_y = round(ny-offset_y);
    for(int j=1; j<n; j++){
        x = points_x[(c1->ind + j)%contour_len];
        y = points_y[(c1->ind + j)%contour_len];
        pivot_left(lower->x, lower->y, x, y, angle, &nx, &ny);
        ny -= offset_y;
        nx -= offset_x;
        int dist_x = round(fabs(nx - prev_x));
        int dist_y = round(fabs(ny - prev_y));
        if(dist_x > 2 || dist_y > 2){
            printf("COOOOCOCOOCC\n");
        }
        else if(dist_x > 1 || dist_y > 1){
            insert_node(&contour, round((ny+prev_y)/2), round((nx+prev_x)/2));
            points_added++;
        }
        insert_node(&contour, round(ny), round(nx));
        prev_x = round(nx);
        prev_y = round(ny);
    }
    puzzle->sides[i].points_num = n+points_added;

    struct Position* positions;
    positions = (struct Position*)malloc((n+points_added) * sizeof(struct Position));
    puzzle->sides[i].positions = positions;
    int cnt = 0;
    while(contour != NULL){
        int x = contour->x;
        int y = contour->y;
        positions[cnt].x = x;
        positions[cnt].y = y;
        cnt++;
        contour = contour->next;
    }
}

void describe_sides(struct Puzzle *puzzle, struct CornerCandidate **corners, int contour_len, int *points_x, int *points_y){
    for(int i=0; i<4; i++){
        struct CornerCandidate *c1 = corners[i];
        struct CornerCandidate *c2 = corners[(i+1)%4];

        puzzle->sides[i].width = point_dist(c1->x, c1->y, c2->x, c2->y);
        puzzle->sides[i].angle_left = 1;
        puzzle->sides[i].angle_right = 2;
        int puzzle_up = describe_type(puzzle, corners, i, contour_len, points_x, points_y);
        describe_side_contour(puzzle, corners, i, contour_len, points_x, points_y, puzzle_up);

        describe_side_heights(&(puzzle->sides[i]));
        describe_shape(&(puzzle->sides[i]));
    }
}

void visualize_rotated_sides(struct BitmapInfo *bitmapInfo, struct Puzzle puzzle){
    for(int i=0; i<bitmapInfo->height; i++){
        for(int j=0; j<bitmapInfo->width; j++){
            set_bitmap(bitmapInfo, j, i, 0);
        }
    }

    int margin_x = 20;
    int margin_y = 20;

    int cur_height = 0;
    for(int i=0; i<4; i++){
        struct PuzzleSide s = puzzle.sides[i];
        int n = s.points_num;

        /*
        double maks = -100.0;
        double mini = 100.0;
        for(int j=0; j<n; j++){
            double y = s.positions[j].y;
            if(y > maks)
                maks = y;
            if(y < mini)
                mini = y;
        }
        double height = maks - mini;
        */

        int height = s.heighest-s.lowest+1;
        //printf("%d\n", (int)height);
        int color = s.type + 5;
        for(int j=0; j<n; j++){
            int x = s.positions[j].x;
            int y = s.positions[j].y;
            //printf("%d %d\n", x, y);
            set_bitmap(bitmapInfo, margin_x + x, cur_height + height + margin_y - y, color);
        }
        set_bitmap(bitmapInfo, margin_x + s.positions[s.left_shape_ind].x, cur_height + height + margin_y - s.positions[s.left_shape_ind].y, 2);
        set_bitmap(bitmapInfo, margin_x + s.positions[s.right_shape_ind].x, cur_height + height + margin_y - s.positions[s.right_shape_ind].y, 2);
        cur_height += (height+margin_y);
    }
}

void visualize_sides(struct BitmapInfo *bitmapInfo){
    for(int i=0; i<bitmapInfo->height; i++)
        for(int j=0; j<bitmapInfo->width; j++){
            int val = get_bitmap(bitmapInfo, j, i);
            if(val == 4 || val == 5 || val == 6){
                for(int y = -5; y<6; y++){
                    for(int x = -5; x<6; x++){
                        if(on_bitmap(bitmapInfo, j+x, i+y) && get_bitmap(bitmapInfo, j+x, i+y)==0)
                            set_bitmap(bitmapInfo, j+x, i+y, val+3);
                    }
                }
            }
        }
/*
    for(int i=0; i<4; i++){
        int c1 = corners[i];
        int c2 = corners[(i+1)%4];
        int n = (c2 - c1 - 1 + contour_len)%contour_len;
        int color = puzzle_sides[i].type + 5;
        for(int j=1; j<n+1; j++){
            int xq = points_x[(c1 + j)%contour_len];
            int yq = points_y[(c1 + j)%contour_len];
            for(int y = -5; y<6; y++){
                for(int x = -5; x<6; x++){
                    if(on_bitmap(bitmapInfo, xq+x, yq+y) && get_bitmap(bitmapInfo, xq+x, yq+y) == 0)
                        set_bitmap(bitmapInfo, xq+x, yq+y, color);
                }
            }
        }
    }
*/
}
