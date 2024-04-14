#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitmap.h"
#include "linked_list.h"

bool is_contour(struct BitmapInfo *bitmapInfo, int x, int y){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    if(y==0 || y==height-1 || x==0 || x==width-1)
        return false;
    if(get_bitmap(bitmapInfo, x+1, y)==0 || get_bitmap(bitmapInfo, x-1, y)==0 || get_bitmap(bitmapInfo, x, y+1)==0 || get_bitmap(bitmapInfo, x, y-1)==0)
        return true;
    return false;
}

int find_contour_neighbours(struct BitmapInfo *bitmapInfo, int x_start, int y_start){
    int group = get_bitmap(bitmapInfo, x_start, y_start);
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
                if(on_bitmap(bitmapInfo, x+j, y+i) && get_bitmap(bitmapInfo, x+j, y+i)==1 && is_contour(bitmapInfo, x+j, y+i)){
                    set_bitmap(bitmapInfo, x+j, y+i, group);
                    cnt++;
                    insert_node(&queue, y+i, x+j);
                }
            }
        }
    }
    return cnt;
}

int get_puzzle_contour(struct BitmapInfo *bitmapInfo, struct Node *point_list){
    int next_group = 2;
    int max_group = 2;
    int max_group_size = 0;
    while(point_list != NULL){
        int y = point_list->y;
        int x = point_list->x;
        if(get_bitmap(bitmapInfo, x, y)==1 && is_contour(bitmapInfo, x, y)){
            set_bitmap(bitmapInfo, x, y, next_group);
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

int leave_only_contour(struct BitmapInfo *bitmapInfo, int group, struct Node **point_list){
    struct Node *prev = NULL;
    int len = 0;
    struct Node *n = *point_list;
    while(n != NULL){
        int y = n->y;
        int x = n->x;
        if(get_bitmap(bitmapInfo, x, y) == group){
            set_bitmap(bitmapInfo, x, y, 1);
            prev = n;
            n = n->next;
            len++;
        }
        else{
            set_bitmap(bitmapInfo, x, y, 0);
            if(prev == NULL){
                delete_start(point_list);
                n = *point_list;
            }
            else{
                delete_next(prev);
                n = prev->next;
            }
        }
    }
    return len;
}

void mark_as_available(struct BitmapInfo *bitmapInfo, int x, int y, int group){
    for(int i=-1; i<2; i++){
        for(int j=-1; j<2; j++){
            if(i==0 && j==0)
                continue;
            if(on_bitmap(bitmapInfo, x+j, y+i) && get_bitmap(bitmapInfo, x+j, y+i)==group){
                set_bitmap(bitmapInfo, x+j, y+i, 1);
            }
        }
    }
}

int get_neighbours_list(struct BitmapInfo *bitmapInfo, struct Node **neighbours, int x, int y){
    int res = 0;
    for(int i=-1; i<2; i++){
        for(int j=-1; j<2; j++){
            if(i==0 && j==0)
                continue;
            if(on_bitmap(bitmapInfo, x+j, y+i) && get_bitmap(bitmapInfo, x+j, y+i)==1){
                insert_node(neighbours, y+i, x+j);
                res++;
            }
        }
    }
    return res;
}

bool can_finish(struct Node *neighbours, int x, int y, struct Node **pts_queue){
    while(neighbours != NULL){
        int n_x = neighbours->x;
        int n_y = neighbours->y;
        if(abs(n_x-x)<2 && abs(n_y-y)<2){
            insert_node(pts_queue, n_y, n_x);
            return true;
        }
        neighbours = neighbours->next;
    }
    return false;
}

int reduce_contour_pts(struct BitmapInfo *bitmapInfo, struct Node **point_list, int contour_len){
    int prev_x = 0;
    int prev_y = 0;
    bool is_forward;
    bool succeed = false;
    struct Node *pts_queue = NULL;
    int cur_cnt;
    while(!succeed){
        int start_x = (*point_list)->x;
        int start_y = (*point_list)->y;
        is_forward = true;
        cur_cnt = 2;
        struct Node *start_neighbours = NULL;
        if(get_neighbours_list(bitmapInfo, &start_neighbours, start_x, start_y) >= 2)
            insert_node(&pts_queue, start_y, start_x);
        set_bitmap(bitmapInfo, start_x, start_y, cur_cnt);
        while(pts_queue != NULL){
            int cur_x = pts_queue->x;
            int cur_y = pts_queue->y;
            if(cur_cnt>contour_len/2 && can_finish(start_neighbours, cur_x, cur_y, &pts_queue)){
                succeed = true;
                break;
            }
            //jesli nie mozesz jeszcze skonczyc
            bool found = false;
            if(is_forward){
                //wszystkie wolne piksele sasiady oznaczam na bitmapie przez cur_len
                //wrzucam pierwszego wolnego piksela sasiada do pts_queue
                for(int i=-1; i<2; i++){
                    for(int j=-1; j<2; j++){
                        if(i==0 && j==0)
                            continue;
                        if(on_bitmap(bitmapInfo, cur_x+j, cur_y+i) && get_bitmap(bitmapInfo, cur_x+j, cur_y+i)==1){
                            if(!found){
                                found = true;
                                insert_node(&pts_queue, cur_y+i, cur_x+j);
                            }
                            set_bitmap(bitmapInfo, cur_x+j, cur_y+i, cur_cnt);
                        }
                    }
                }
                //jesli nie znalazlem zadnego wolnego piksela
                if(!found){
                    is_forward = false;
                    delete_start(&pts_queue);
                    prev_x = cur_x;
                    prev_y = cur_y;
                    cur_cnt--;
                }
                else
                    cur_cnt++;
            }
            else{
                //wszystkie piksele sasiady sa juz oznaczone na bitmapie przez cur_len
                //poprzez ustawienie wartoœci i oraz j w ten sposob, przeszukuje wszystkie
                //nieprzeanalizowane jeszcze piksele sasiady. Jesli taki sie znajdzie, dodaje
                //go do pts_queue, zwiekszam cur_len i ustawiam forward=true.
                for(int i=-1; i<2; i++){
                    for(int j=-1; j<2; j++){
                        if(i < prev_y-cur_y || (i == prev_y-cur_y && j <= prev_x-cur_x))
                            continue;
                        if(i==0 && j==0)
                            continue;
                        if(on_bitmap(bitmapInfo, cur_x+j, cur_y+i) && get_bitmap(bitmapInfo, cur_x+j, cur_y+i)==cur_cnt){
                            if(!found){
                                found = true;
                                insert_node(&pts_queue, cur_y+i, cur_x+j);
                            }
                        }
                    }
                }
                if(!found){
                    delete_start(&pts_queue);
                    prev_x = cur_x;
                    prev_y = cur_y;
                    mark_as_available(bitmapInfo, cur_x, cur_y, cur_cnt);
                    cur_cnt--;
                }
                else{
                    is_forward = true;
                    cur_cnt++;
                }
            }
        }
        if(!succeed){
            set_bitmap(bitmapInfo, start_x, start_y, 0);
            delete_start(point_list);
        }
        free_list(start_neighbours);
    }
    struct Node *n = *point_list;
    while(n != NULL){
        int x = n->x;
        int y = n->y;
        set_bitmap(bitmapInfo, x, y, 0);
        n = n->next;
    }
    free_list(*point_list);

    //nalezy sprawdzic czy punkty na pts_queue sa zgodnie z ruchem wskazowek
    int v1 = cur_cnt/3;
    int v2 = cur_cnt/3*2;
    int x0 = pts_queue->x;
    int y0 = pts_queue->y;
    int x1, y1, x2, y2;

    int counter = 0;
    n = pts_queue;
    while(n != NULL){
        int x = n->x;
        int y = n->y;
        if(counter == v1){
            x1 = x;
            y1 = y;
        }
        else if(counter == v2){
            x2 = x;
            y2 = y;
        }
        set_bitmap(bitmapInfo, x, y, 1);
        n = n->next;
        counter++;
    }

    int res = (x1-x0)*(y2-y0)-(y1-y0)*(x2-x0);
    if(res < 0)
        reverse_list(&pts_queue);

    *point_list = pts_queue;
    return cur_cnt;
}


