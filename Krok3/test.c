#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pomoc.h"
#include "linked_list.h"

//mozna potem dodac watki

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

void read_bitmap(struct BitmapInfo *bitmapInfo, FILE* finptr, struct Node **point_list){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            int r, g, b;
            fscanf(finptr, "%d %d %d", &r, &g, &b);
            if(puzzle_pixel(r, g, b)){
                set_bitmap(bitmapInfo, x, y, 1);
                //bitmap[y][x] = 1;
                insert_node(point_list, y, x);
            }
            else
                set_bitmap(bitmapInfo, x, y, 0);
                //bitmap[y][x] = 0;
        }
    }
}

bool is_contour(struct BitmapInfo *bitmapInfo, int x, int y){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    if(y==0 || y==height-1 || x==0 || x==width-1)
        return false;
    if(get_bitmap(bitmapInfo, x+1, y)==0 || get_bitmap(bitmapInfo, x-1, y)==0 || get_bitmap(bitmapInfo, x, y+1)==0 || get_bitmap(bitmapInfo, x, y-1)==0)
        return true;
    //if(bitmap[y][x+1]==0 || bitmap[y][x-1]==0 || bitmap[y+1][x]==0 || bitmap[y-1][x]==0)
    //    return true;
    return false;
}

bool on_bitmap(struct BitmapInfo *bitmapInfo, int x, int y){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    return (x>=0 && x<width && y>=0 && y<height);
}

int find_contour_neighbours(struct BitmapInfo *bitmapInfo, int x_start, int y_start){
    //int group = bitmap[y_start][x_start];
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
                    //bitmap[y+i][x+j] = group;
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

void bitmap_to_file(struct BitmapInfo *bitmapInfo, FILE* foutptr){
    int width = bitmapInfo->width;
    int height = bitmapInfo->height;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(get_bitmap(bitmapInfo, x, y) == 1)
                fprintf(foutptr, "%d %d %d\n", 0, 0, 0);
            else
                fprintf(foutptr, "%d %d %d\n", 255, 255, 255);
        }
    }
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
    //mozna na koniec sprawdzic czy ostatni dodany nie graniczy z drugim - moza usunac pierwszy a moze tez ten drugi
    int prev_x = 0;
    int prev_y = 0;
    bool forward;
    bool succeed = false;
    struct Node *pts_queue = NULL;
    int cur_cnt;
    while(!succeed){
        int start_x = (*point_list)->x;
        int start_y = (*point_list)->y;
        forward = true;
        cur_cnt = 2;
        struct Node *start_neighbours = NULL;
        if(get_neighbours_list(bitmapInfo, &start_neighbours, start_x, start_y) >= 2)
            insert_node(&pts_queue, start_y, start_x);
        set_bitmap(bitmapInfo, start_x, start_y, cur_cnt);
        while(pts_queue != NULL){
            int cur_x = pts_queue->x;
            int cur_y = pts_queue->y;
            //printf("%d %d %d %d %d %d\n", cur_cnt, cur_x, cur_y, forward, prev_y-cur_y, prev_x-cur_x);
            if(cur_cnt>contour_len/2 && can_finish(start_neighbours, cur_x, cur_y, &pts_queue)){
                succeed = true;
                break;
            }
            //jesli nie mozesz jeszcze skonczyc
            bool found = false;
            if(forward){
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
                    forward = false;
                    delete_start(&pts_queue);
                    prev_x = cur_x;
                    prev_y = cur_y;
                    cur_cnt--;
                }
                else
                    cur_cnt++;
            }
            else{
                //wszystkie piksele s¹siady s¹ ju¿ oznaczone na bitmapie przez cur_len
                //poprzez ustawienie wartoœci i oraz j w ten sposob, przeszukuje wszystkie
                //nieprzeanalizowane jeszcze piksele s¹siady. Jeœli taki siê znajdzie, dodaje
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
                    forward = true;
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
    n = pts_queue;
    while(n != NULL){
        int x = n->x;
        int y = n->y;
        set_bitmap(bitmapInfo, x, y, 1);
        n = n->next;
    }
    *point_list = pts_queue;
    return cur_cnt;
}

double distance_to_line(int x, int y, double a, double b, double c){
    return fabs(a*x + b*y + c)/sqrt(a*a+b*b);
}

void find_longest_lines(int *points_x, int *points_y,int contour_len, int *longest_lines){
    int max_error = 10;
    int max_dist = 1; //max dystans punktu od prostej
    for(int i=0; i<contour_len; i++){
        //znajdz najwieksze takie j ze mozna polaczyc j kolejnych punktow jedna prosta
        //ze wzgledu na problemy ze skanami pozwalamy by punkty na chwile odeszly od prostej
        //o ile znowu na nia wroca (max error_len punktow zanim wroci)

        //zalozmy ze ta prosta bedzie sie skladala z countour_len/2 punktow
        //sprawdz czy sie da - jesli nie to robimy binsearch, czyli sprawdzamy dla contour_len/4
        //jesli sie da to zwiekszamy liczbe punkotow o polowe rozwazanego przedzialu
        //prosta latwo wyznaczyc z pierwszego i ostatniego punktu ktore musza sie na niej znajdowac
        //pytanie czy pierwsze np 5 punktow musi byc na prostej - wpp na rogach najdluzsza prosta bedzie dla punktu chwile przed zakretem
        int x0 = points_x[i];
        int y0 = points_y[i];
        int s = 2;
        int e = contour_len;
        while(e-s > 1){
            int m = (s+e)/2;
            int xm = points_x[(i+m)%contour_len];
            int ym = points_y[(i+m)%contour_len];
            //sprawdzamy czy da sie utworzyc dobra prosta przez nastepne m punktow
            //prosta w postaci ogolnej ma postac ax+by+c = 0
            double a=1.0;
            double b=0.0;
            double c=-1.0*(double)x0;
            if(x0!=xm){
                a = (double)(ym-y0)/(x0-xm);
                b = 1.0;
                c = -1.0*(double)(a*x0+y0);
            }
            int error_counter = 0;
            bool succeed = true;
            for(int j=1; j<m; j++){
                int xj = points_x[(i+j)%contour_len];
                int yj = points_y[(i+j)%contour_len];
                if(distance_to_line(xj, yj, a, b, c) > max_dist){
                    error_counter++;
                    if(error_counter >= max_error){
                        succeed = false;
                        break;
                    }
                }
                else{
                    error_counter = 0;
                }
            }
            if(succeed)
                s = m;
            else
                e = m;
        }
        longest_lines[i] = s;
    }
}

void find_corners(struct BitmapInfo *bitmapInfo, struct Node *point_list, int contour_len, struct Node **corners){
    int points_x[contour_len];
    int points_y[contour_len];
    int i=0;
    while(point_list != NULL){
        points_x[i] = point_list->x;
        points_y[i] = point_list->y;
        point_list = point_list->next;
        i++;
    }
    //znajdz najdluzsze proste odcinki a potem rogi
    int longest_lines[contour_len];
    find_longest_lines(points_x, points_y, contour_len, longest_lines);
    for(int i=0; i<contour_len; i++){
        int v=longest_lines[i];
        printf("%d. %d %d to %d %d wynik %d\n", i, points_x[i], points_y[i], points_x[(i+v)%contour_len], points_y[(i+v)%contour_len], v);
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
        struct BitmapInfo bitmapInfo;
        bitmapInfo.width = width;
        bitmapInfo.height = height;
        bitmapInfo.bitmap = (int*)bitmap;
        read_bitmap(&bitmapInfo, finptr, &point_list);
        int contour_num = get_puzzle_contour(&bitmapInfo, point_list);
        int contour_len = leave_only_contour(&bitmapInfo, contour_num, &point_list);
        contour_len = reduce_contour_pts(&bitmapInfo, &point_list, contour_len);
        struct Node *corners = NULL;
        find_corners(&bitmapInfo, point_list, contour_len, &corners);

        printf("%d\n", i);
        bitmap_to_file(&bitmapInfo, foutptr);
        fclose(finptr);
        fclose(foutptr);
        free_list(point_list);
    }
}
