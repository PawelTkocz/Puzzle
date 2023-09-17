#include <stdio.h>
#include <stdlib.h>

struct CornerCandidate{
    int ind;
    int x;
    int y;
    int end_left_x;
    int end_left_y;
    int end_right_x;
    int end_right_y;
};

struct PuzzleSide{
    int type;
    //-1 wklesly 0 bok 1 wypukly
    double width;
};

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
            //int x = points_x[i];
            //int y = points_y[i];
            double ny = 1;
            //pivot_right(lower->x, lower->y, x, y, &nx, &ny);
            if(ny > upper_line)
                upper_cnt++;
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

