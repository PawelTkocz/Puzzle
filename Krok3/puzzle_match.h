#include <stdio.h>
#include <stdlib.h>
#define NO_MATCH 20000
#define PROBABLY_NO_MATCH 2500


//opisac krawedzie zgodnie z ruchem wskazowek zegara a nie raz w jedna raz w druga

int get_width_points(int width1, int width2){
    int longer = width1;
    int shorter = width2;
    if(width2 > width1){
        longer = width2;
        shorter = width1;
    }

    int max_diff = 10;
    int diff = longer-shorter;

    if(diff > max_diff)
        return PROBABLY_NO_MATCH;

    int alfa = 50;
    int margin = 3;
    if(diff < margin)
        return 0;
    else
        return alfa*(diff-margin)*(diff-margin);
}

int get_shape_points(struct PuzzleSide s1, struct PuzzleSide s2){
    struct Position *positions1 = s1.positions;
    struct Position *positions2 = s2.positions;
    int left_shape_x1 = positions1[s1.left_shape_ind].x;
    int left_shape_x2 = positions2[s2.left_shape_ind].x;
    int right_shape_x1 = positions1[s1.right_shape_ind].x;
    int right_shape_x2 = positions2[s2.right_shape_ind].x;

    int diff_left = abs(left_shape_x1-left_shape_x2);
    int diff_right = abs(right_shape_x1-right_shape_x2);

    int max_diff = 25;

    if(diff_left > max_diff || diff_right > max_diff)
        return PROBABLY_NO_MATCH;

    int alfa = 50;
    int margin = 5;
    if(diff_left < margin && diff_right < margin)
        return 0;
    return alfa*(diff_left+diff_right);
}

double get_height_points(struct PuzzleSide s1, struct PuzzleSide s2){
    int height1 = s1.heighest - s1.lowest + 1;
    int height2 = s2.heighest - s2.lowest + 1;

    int max_diff = 10;
    int diff = abs(height1-height2);

    if(diff>max_diff)
        return PROBABLY_NO_MATCH;

    double alfa = 50.0;
    int margin = 3;
    if(diff < margin)
        return 0;
    else
        return alfa*(diff-margin)*(diff-margin);
}

int get_side_comparisson_points(struct PuzzleSide *s1, struct PuzzleSide *s2){

    int best_score = 10000;

    int is_print = 0;
    int width1 = s1->width;
    int width2 = s2->width;

    struct PuzzleSide *longer = s1;
    struct PuzzleSide *shorter = s2;

    if(width2 > width1){
        shorter = s1;
        longer = s2;
    }


    for(int v=0; v<13; v+=2){
        int margin_y = v-6;
        //margin_y [-6, 6]
        //wysokosc nad lub pod jaka zaczyna sie rysowanie boku krotszego w stosunku do dluzszego
        for(int q=0; q<7; q++){
            int margin_x = q-3;
            //margin_x[-3, 3];
            if(abs(margin_x + shorter->width-longer->width)>5)
                continue;


            int height_above1 = longer->heighest;
            int height_above2 = shorter->heighest + margin_y;
            int max_height_above = height_above1;
            if(height_above2>height_above1)
                max_height_above = height_above2;

            int height_under1 = longer->lowest;
            int height_under2 = shorter->lowest + margin_y;
            int min_height_under = height_under1;
            if(height_under2<height_under1)
                min_height_under = height_under2;

            int min_width = 0;
            if(margin_x<0)
                min_width = margin_x;

            int max_width = longer->width;
            if(margin_x+shorter->width > max_width)
                max_width = margin_x+shorter->width;

            int bitmap_width = max_width-min_width;
            int bitmap_height = max_height_above-min_height_under+2;

            int bitmap[bitmap_height][bitmap_width];
            for(int i=0; i<bitmap_height; i++)
                for(int j=0; j<bitmap_width; j++)
                    bitmap[i][j] = 0;

            int start_y = -1*min_height_under + 1;
            int start_x = -1*min_width;
            int n1 = longer->points_num;
            for(int i=0; i<n1; i++){
                int x = longer->positions[i].x;
                int y = longer->positions[i].y;
                bitmap[start_y+y][start_x+x] = 2;
            }

            for(int i=longer->positions[0].y+start_y; i>=0; i--)
                bitmap[i][start_x+longer->positions[0].x] = 2;
            for(int i=longer->positions[n1-1].y+start_y; i>=0; i--)
                bitmap[i][start_x+longer->positions[n1-1].x] = 2;



            if(is_print){
                for(int i=bitmap_height-1; i>=0; i--){
                    for(int j=0; j<bitmap_width; j++)
                        printf("%d", bitmap[i][j]);
                    printf("\n");
                }
                printf("\n");
            }

            int directions[4] = {0, 0, -1, 1};
            struct Node *queue = NULL;
            insert_node(&queue, 0, bitmap_width/2);
            int first_cnt = 1;
            bitmap[0][bitmap_width/2] = 1;
            while(queue != NULL){
                int y = queue->y;
                int x = queue->x;
                delete_start(&queue);
                for(int p=0; p<4; p++){
                    int i = directions[p];
                    int j = directions[3-p];
                    if(x+j<0 || x+j>=bitmap_width || y+i < 0 || y+i >= bitmap_height)
                        continue;
                    if(bitmap[y+i][x+j]==0){
                        bitmap[y+i][x+j]=1;
                        first_cnt++;
                        insert_node(&queue, y+i, x+j);
                    }
                }
            }

            if(is_print){
                for(int i=bitmap_height-1; i>=0; i--){
                    for(int j=0; j<bitmap_width; j++)
                        printf("%d", bitmap[i][j]);
                    printf("\n");
                }
                printf("\n");
            }

            //printf("first_cnt %d\n", first_cnt);

            if(bitmap[bitmap_height-1][0] == 1 || bitmap[bitmap_height-1][bitmap_width-1] == 1)
                printf("ZAPELNIONO CALA BITMAPE\n");

            int n2 = shorter->points_num;
            for(int i=0; i<n2; i++){
                int x = shorter->positions[i].x;
                int y = shorter->positions[i].y;
                bitmap[start_y+margin_y+y][start_x+margin_x+x] = 3;
            }
            for(int i=shorter->positions[0].y+start_y+margin_y; i>=0; i--)
                bitmap[i][start_x+margin_x+shorter->positions[0].x] = 3;
            for(int i=shorter->positions[n2-1].y+start_y+margin_y; i>=0; i--)
                bitmap[i][start_x+margin_x+shorter->positions[n2-1].x] = 3;

            insert_node(&queue, 0, bitmap_width/2);
            int new_points = 0;
            bitmap[0][bitmap_width/2] = 4;
            first_cnt--;
            while(queue != NULL){
                int y = queue->y;
                int x = queue->x;
                delete_start(&queue);
                for(int p=0; p<4; p++){
                    int i = directions[p];
                    int j = directions[3-p];
                    if(i==0 && j==0)
                        continue;
                    if(x+j<0 || x+j>=bitmap_width || y+i < 0 || y+i >= bitmap_height)
                        continue;
                    if(bitmap[y+i][x+j]==0 || bitmap[y+i][x+j]==1 || bitmap[y+i][x+j]==2){
                        insert_node(&queue, y+i, x+j);
                        if(bitmap[y+i][x+j]==0 || bitmap[y+i][x+j]==2){
                            new_points++;
                            bitmap[y+i][x+j]=8;
                        }
                        else{
                            first_cnt--;
                            bitmap[y+i][x+j]=4;
                        }
                    }
                }
            }
            //printf("new_points first_cnt %d %d\n", new_points, first_cnt);

            if(is_print){
                for(int i=bitmap_height-1; i>=0; i--){
                    for(int j=0; j<bitmap_width; j++)
                        printf("%d", bitmap[i][j]);
                    printf("\n");
                }
                printf("\n");
            }


            if(bitmap[bitmap_height-1][0] == 4 || bitmap[bitmap_height-1][bitmap_width-1] == 4)
                printf("DRUGI ZAPELNIL NAJPRAWDOPODOBNIEJ CALA BITMAPE\n");

            int res = first_cnt+new_points;
            if(res < best_score)
                best_score = res;
        }
    }

    return best_score;
}

double puzzle_match_val(struct PuzzleSide *s1, struct PuzzleSide *s2){
    if(s1->type == 0 || s2->type == 0)
        return NO_MATCH;
    if(s1->type + s2->type != 0)
        return NO_MATCH;

    double width_points = get_width_points(s1->width, s2->width);
    //[0, 2500]

    double shape_points = get_shape_points(*s1, *s2);
    //[0, 2500]

    double height_points = get_height_points(*s1, *s2);
    //[0, 2500]

    int side_comparisson_points = get_side_comparisson_points(s1, s2);
    //[0, 10000]

    //printf("%lf %lf %lf %d\n", width_points, shape_points, height_points, side_comparisson_points);
    int sum = width_points + shape_points + height_points + side_comparisson_points;
    //[0, 175000]
    return sum;
}
