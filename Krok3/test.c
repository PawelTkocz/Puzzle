#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pomoc.h"
#include "linked_list.h"
#include "puzzle_placement.h"

//mozna potem dodac watki

struct PuzzleSide{
    int type;
    //-1 wklesly 0 bok 1 wypukly
    double width;
};

struct CornerCandidate{
    int ind;
    int x;
    int y;
    int end_left_x;
    int end_left_y;
    int end_right_x;
    int end_right_y;
};

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
            int v = get_bitmap(bitmapInfo, x, y);
            if(v == 1)
                fprintf(foutptr, "%d %d %d\n", 0, 0, 0);
            else if(v == 3)
                fprintf(foutptr, "%d %d %d\n", 255, 0, 255);
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

double find_angle(int x0, int y0, int x1, int y1, int x2, int y2){
    double a1 = x1-x0;
    double b1 = y1-y0;
    double a2 = x2-x0;
    double b2 = y2-y0;
    double angle = acos((a1*a2+b1*b2)/(point_dist(x0, y0, x1, y1)*point_dist(x0, y0, x2, y2))) * 180 / 3.141592;
    return angle;
}

double ostry_angle(double angle){
    if(angle > 90.0)
        angle = 180.0 - angle;
    return angle;
}

void find_longest_lines(int *points_x, int *points_y, int contour_len, int **tab){
    double *longest_l_len = (double *)tab[0];
    int *longest_l_end = tab[1];
    double *longest_r_len = (double *)tab[2];
    int *longest_r_end = tab[3];

    int max_error = 35;
    int max_dist = 2; //max dystans punktu od prostej
    int first = 5;

    for(int dir=-1; dir<2; dir+=2){
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
                //spawdzic czy ostatnie n punktow lezy na prostej

                int xm = points_x[(contour_len+i+dir*m)%contour_len];
                int ym = points_y[(contour_len+i+dir*m)%contour_len];


                if(m>first){
                    double angle = find_angle(x0, y0, xm, ym, points_x[(contour_len+i+dir*first)%contour_len], points_y[(contour_len+i+dir*first)%contour_len]);
                    if(x0 == 100 && y0 == 238){
                        //printf("%d %d %d %lf pop\n", xm, ym, dir, angle);
                    }
                    if(angle > 30){
                        e = m;
                        continue;
                    }
                }

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
                int highest_error = 0;
                int line_counter = 0;
                bool succeed = true;
                for(int j=1; j<m; j++){
                    int xj = points_x[(contour_len+i+dir*j)%contour_len];
                    int yj = points_y[(contour_len+i+dir*j)%contour_len];
                    if(distance_to_line(xj, yj, a, b, c) > max_dist){
                        line_counter = 0;
                        error_counter++;
                        if(error_counter >= max_error){
                            succeed = false;
                            break;
                        }
                    }
                    else{
                        if(highest_error < error_counter)
                            highest_error = error_counter;
                        error_counter = 0;
                        line_counter++;
                        if(line_counter >= highest_error/2)
                            highest_error = 0;
                    }
                }
                if(highest_error==0 && succeed){
                    s = m;
                }
                else
                    e = m;
            }
            if(dir == -1){
                longest_l_len[i] = point_dist(x0, y0, points_x[(contour_len+i-s)%contour_len], points_y[(contour_len+i-s)%contour_len]);
                longest_l_end[i] = s;
                if(x0 == 100 && y0 == 238){
                    //printf("%d %d %lf pop\n", points_x[longest_l_end[i]], points_y[longest_l_end[i]], longest_l_len[i]);
                }
            }
            else{
                longest_r_len[i] = point_dist(x0, y0, points_x[(i+s)%contour_len], points_y[(i+s)%contour_len]);
                longest_r_end[i] = s;
                if(x0 == 100 && y0 == 238){
                    //printf("%d %d %lf pop\n", points_x[longest_r_end[i]], points_y[longest_r_end[i]], longest_r_len[i]);
                }
            }
        }
    }
}

int find_max_line(int **tab, int *points_x, int *points_y, int contour_len, bool *available){
    double *sum = (double *)tab[0];
    int *longest_l_end = tab[1];
    int *longest_r_end = tab[2];

    double max_val = 0.0;
    int max_ind = 0;
    for(int i=0; i<contour_len; i++){
        if(!available[i])
            continue;
        if(sum[i] > max_val){
            max_val = sum[i];
            max_ind = i;
        }
    }
    //oznaczamy punkyt jako not available

    available[max_ind] = false;

    for(int i=1; i<longest_l_end[max_ind]-5; i++)
        available[(contour_len+max_ind-i)%contour_len] = false;
    for(int i=1; i<longest_r_end[max_ind]-5; i++)
        available[(max_ind+i)%contour_len] = false;

    return max_ind;
}

bool are_corners(struct CornerCandidate *c1, struct CornerCandidate *c2, struct CornerCandidate *c3, struct CornerCandidate *c4, int contour_len){
    struct CornerCandidate *candidates[4] = {c1, c2, c3, c4};
    for(int i=0; i<3; i++){
        int min_ind = i;
        int min_val = candidates[i]->ind;
        for(int j=i+1; j<4; j++){
            if(candidates[j]->ind < min_val){
                min_val = candidates[j]->ind;
                min_ind = j;
            }
        }
        struct CornerCandidate *temp = candidates[i];
        candidates[i] = candidates[min_ind];
        candidates[min_ind] = temp;
    }

    for(int i=0; i<4; i++){
        int ind1 = (4+i-1)%4;
        int ind2 = (i+1)%4;
        double angle = find_angle(candidates[i]->x, candidates[i]->y, candidates[ind1]->x, candidates[ind1]->y, candidates[ind2]->x, candidates[ind2]->y);
        angle = ostry_angle(angle);
        if(angle<75)
            return false;
        //sprawdz czy wektory na siebie wskazuja
        //i jesli wskazuja to czy sumy obu dlugosci sa conajmniej polowa ich odleglosci
        double angle1 = find_angle(candidates[i]->x, candidates[i]->y, candidates[ind2]->x, candidates[ind2]->y, candidates[i]->end_right_x, candidates[i]->end_right_y);
        if(angle1 > 45)
            return false;
        double angle2 = find_angle(candidates[ind2]->x, candidates[ind2]->y, candidates[i]->x, candidates[i]->y, candidates[ind2]->end_left_x, candidates[ind2]->end_left_y);
        if(angle2 > 45)
            return false;
        double side_len = point_dist(candidates[i]->x, candidates[i]->y, candidates[ind2]->x, candidates[ind2]->y);
        if( (contour_len + candidates[ind2]->ind - candidates[i]->ind)%contour_len > side_len*2.5)
            return false;
    }

    return true;
}

bool corners_found(struct CornerCandidate *candidates, int n, int *final_corners, int contour_len){
    if(n<4)
        return false;
    for(int a=0; a<n-3; a++)
        for(int b=a+1; b<n-2; b++)
            for(int c=b+1; c<n-1; c++)
                for(int d=c+1; d<n; d++){
                    /*
                    int xs[4] = {candidates[a].x, candidates[b].x, points_x[candidates[c]], points_x[candidates[d]]};
                    int ys[4] = {points_y[candidates[a]], points_y[candidates[b]], points_y[candidates[c]], points_y[candidates[d]]};
                    if(is_rectangle(xs, ys)){
                        //przyda sie dodatkowo sprawdzenie czy pomiedzy dwoma rogami sa maksymalnie dwaj kandydaci na rogi
                        candidates[0] = candidates[a];
                        candidates[1] = candidates[b];
                        candidates[2] = candidates[c];
                        candidates[3] = candidates[d];
                        return true;
                    }
                    */
                    if(are_corners(&candidates[a], &candidates[b], &candidates[c], &candidates[d], contour_len)){
                        final_corners[0] = a;
                        final_corners[1] = b;
                        final_corners[2] = c;
                        final_corners[3] = d;
                        return true;
                    }
                }
    return false;
    //kiedy dodajemy kandydata na rog trzeba zapamietac w ktora strone skierowane sa ramiona katu
    //i kiedy sprawdzamy czy kandydaci tworza prostokat trzeba dodatkowo sprawdzic czy te wektory
    //wskazuja we wlasciwa storone
}

void describe_side(int i, struct PuzzleSide *sides, struct CornerCandidate *c1, struct CornerCandidate *c2, int contour_len, int *points_x, int *points_y){
    double side_len = point_dist(c1->x, c1->y, c2->x, c2->y);
    sides[i].width = side_len;
    if( (contour_len + c2->ind - c1->ind)%contour_len < side_len*1.2)
        sides[i].type = 0;
    else{
        //trzeba stwierdzic czy wypukly czy wklesly
        //poprowadz dwie rownolegle do boku proste, jedna pod jedna nad (10 px np)
        //sprawdz ile pikseli jest pod nizsza i nad wyzsza
        struct CornerCandidate *lower = c1;
        struct CornerCandidate *upper = c2;
        if(c2->y < c1->y){
            lower = c2;
            upper = c1;
        }
        double angle = find_angle(lower->x, lower->y, upper->x, lower->y, upper->x, upper->y);
        //trzeba zdeycdowac czy rotacja bedzie w prawo czy w lewo i czy o angle czy 180-angle
        //zalozmy ze rotacja zawsze w prawo
        if(upper->x < lower->x)
            angle = 180.0 - angle;
        int margin = 5;
        int upper_line = lower->x + margin;
        int lower_line = lower->x - margin;
        int lower_cnt = 0;
        int upper_cnt = 0;
        for(int i=c1->ind+1; i<c2->ind; i++){
            int x = points_x[i];
            int y = points_y[i];
            double ny;
            pivot_right(lower->x, lower->y, x, y, &nx, &ny);
            if(ny > upper_line)
                upper_cnt++
            else if(ny < lower_line)
                lower_cnt++;
        }

    }


}

void find_corners(struct BitmapInfo *bitmapInfo, struct Node *point_list, int contour_len, int *crnrs, struct PuzzleSide *sides){
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
    double longest_r_len[contour_len];
    int longest_r_end[contour_len];
    double longest_l_len[contour_len];
    int longest_l_end[contour_len];
    int *temp[] = {(int*)longest_l_len, longest_l_end, (int*)longest_r_len, longest_r_end};
    find_longest_lines(points_x, points_y, contour_len, temp);
    /*
    for(int i=0; i<contour_len; i++){
        double l1 = longest_l_len[i];
        double l2 = longest_r_len[i];
        int d1 = longest_l_end[i];
        int d2 = longest_r_end[i];
        int x0 = points_x[i];
        int y0 = points_y[i];
        int x1 = points_x[(contour_len+i-d2)%contour_len];
        int y1 = points_y[(contour_len+i-d2)%contour_len];
        int x2 = points_x[(i+d1)%contour_len];
        int y2 = points_y[(i+d1)%contour_len];
        double v = l1+l2;
        printf("%d (%d %d) -> %lf (%d %d) + %lf (%d %d) = %lf\n", i, x0, y0, l2, x1, y1, l1, x2, y2, v);
    }
    */

    bool available[contour_len];
    for(int i=0; i<contour_len; i++)
        available[i] = true;
    double sum[contour_len];
    for(int i=0; i<contour_len; i++)
        sum[i] = longest_r_len[i] + longest_l_len[i];
    int *tab[] = {(int*)sum, longest_l_end, longest_r_end};

    int cnt = 0;
    //int candidates[15];
    struct CornerCandidate candidates[15];
    int final_corners[4];
    while(cnt < 15){
        //printf("re %d\n", available[221]);
        //znajdz najdluzszy odcinek
        //oznacz dwa jego konce w available przez 2
        //punkty pomiedzy nimi oznacz w available przez 0
        //kiedy oznaczasz cos przez 2 sprawdz czy nie ma w promieniu r innej 2
        //jesli jest to wyciagnij sredni punkt z tych dwojek i dodaj go do corners_candidates
        int ind = find_max_line(tab, points_x, points_y, contour_len, available);

        int x0 = points_x[ind];
        int y0 = points_y[ind];
        int d1 = longest_l_end[ind];
        int d2 = longest_r_end[ind];
        int x1 = points_x[(contour_len+ind-d1)%contour_len];
        int y1 = points_y[(contour_len+ind-d1)%contour_len];
        int x2 = points_x[(ind+d2)%contour_len];
        int y2 = points_y[(ind+d2)%contour_len];

        double angle = find_angle(x0, y0, x1, y1, x2, y2);
        angle = ostry_angle(angle);
        //printf("%d %d %lf\n", points_x[ind], points_y[ind], angle);
        //printf("%d %d %d %d\n", x1, y1, x2, y2);
        if(angle > 55){

            //printf("%d %d %lf\n", points_x[ind], points_y[ind], angle);
            //printf("%d %d %d %d\n", x1, y1, x2, y2);

            candidates[cnt].ind = ind;
            candidates[cnt].x = x0;
            candidates[cnt].y = y0;
            candidates[cnt].end_left_x = x1;
            candidates[cnt].end_left_y = y1;
            candidates[cnt].end_right_x = x2;
            candidates[cnt].end_right_y = y2;

            cnt++;
            if(corners_found(candidates, cnt, final_corners, contour_len))
                break;
        }
    }
    for(int i=0; i<4; i++){
        set_bitmap(bitmapInfo, candidates[final_corners[i]].x, candidates[final_corners[i]].y, 2);
        //printf("%d %d\n", points_x[candidates[i]], points_y[candidates[i]]);
        crnrs[i] = candidates[final_corners[i]].ind;
    }
    select_sort_two(4, crnrs, final_corners);
    for(int i=0; i<4; i++){
        describe_side(i, sides, &candidates[final_corners[i]], &candidates[final_corners[(i+1)%4]], contour_len, points_x, points_y);
    }
}

void visualize_corners(struct BitmapInfo *bitmapInfo){
    for(int i=0; i<bitmapInfo->height; i++)
        for(int j=0; j<bitmapInfo->width; j++){
            if(get_bitmap(bitmapInfo, j, i) == 2){
                for(int y = -15; y<16; y++){
                    for(int x = -15; x<16; x++){
                        if(on_bitmap(bitmapInfo, j+x, i+y))
                            set_bitmap(bitmapInfo, j+x, i+y, 3);
                    }
                }
            }
        }
}

int main(){
    int puzzle_pieces;
    printf("Podaj liczbe puzzli: ");
    scanf("%d", &puzzle_pieces);

    //kazdy puzzel

    int contour_length[puzzle_pieces];
    int* points_x[puzzle_pieces];
    int* points_y[puzzle_pieces];
    int* corners[puzzle_pieces];
    struct PuzzleSide* sides[puzzle_pieces];

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

        int *crnrs;
        struct PuzzleSide *puzzle_sides;
        crnrs = (int*)malloc(4*sizeof(int));
        puzzle_sides = (struct PuzzleSide*)malloc(4*sizeof(struct PuzzleSide));
        find_corners(&bitmapInfo, point_list, contour_len, crnrs, puzzle_sides);
        corners[i] = crnrs;
        sides[i] = puzzle_sides;
        contour_length[i] = contour_len;
        int *pts_x;
        pts_x = (int*)malloc(contour_len*sizeof(int));
        int *pts_y;
        pts_y = (int*)malloc(contour_len*sizeof(int));
        int j=0;
        struct Node* n = point_list;
        while(n != NULL){
            pts_x[j] = n->x;
            pts_y[j] = n->y;
            n = n->next;
            j++;
        }
        points_x[i] = pts_x;
        points_y[i] = pts_y;

        visualize_corners(&bitmapInfo);
        bitmap_to_file(&bitmapInfo, foutptr);
        fclose(finptr);
        fclose(foutptr);
        free_list(point_list);
        printf("%d\n", i);
    }


    for(int i=0; i<puzzle_pieces; i++){
        printf("%d\n", i);
        int n = contour_length[i];
        for(int j=0; j<4; j++){
            if(corners[i][j] < 0)
                printf("%d ", corners[i][j]);
            if(sides[i][j].type == 0)
                printf("jej\n");
        }
        for(int j=0; j<n; j++){
            if(points_x[i][j] < 0 || points_y[i][j] < 0)
                printf("(%d %d) ", points_x[i][j], points_y[i][j]);
        }
    }

}
