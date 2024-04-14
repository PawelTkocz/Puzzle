#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bitmap.h"
#include "structs.h"
#include "puzzle_match.h"
#include "corners.h"

struct FinalStruct{
    struct BitmapInfo *puzzleSolution;
    struct BitmapInfo *sideUpward;
    int puzzle_pieces;
    struct Puzzle *puzzles;
};

struct PuzzleNode{
    int ind;
    struct PuzzleNode *next;
};

struct BorderPath{
    int len;
    struct PuzzleNode *first_puzzle;
    struct PuzzleNode *last_puzzle;
    struct BorderPath *prev;
    struct BorderPath *next;
};

bool is_in(int a, int tab[4]){
    for(int i=0; i<4; i++)
        if(a == tab[i])
            return true;
    return false;
}

struct PuzzleNode* malloc_Puzzlenode(int ind, struct PuzzleNode *next){
    struct PuzzleNode *new_node = (struct PuzzleNode *)malloc(sizeof(struct PuzzleNode));
    new_node->ind = ind;
    new_node->next = next;
    return new_node;
}

struct BorderPath* malloc_BorderPath(int left, int right){
    struct BorderPath *new_border_path = (struct BorderPath *)malloc(sizeof(struct BorderPath));
    new_border_path->len = 2;
    struct PuzzleNode *pn2 = malloc_Puzzlenode(right, NULL);
    struct PuzzleNode *pn1 = malloc_Puzzlenode(left, pn2);
    new_border_path->first_puzzle = pn1;
    new_border_path->last_puzzle = pn2;
    new_border_path->prev = NULL;
    new_border_path->next = NULL;
    return new_border_path;
}

void delete_border_path(struct BorderPath *bp, struct BorderPath **head){
    if(bp == NULL)
        return;

    if(bp->next != NULL)
        bp->next->prev = bp->prev;

    if(bp->prev != NULL)
        bp->prev->next = bp->next;
    else
        *head = bp->next;

    free(bp);
}

bool great_match(int res1, int res2){
    if(res2 > res1)
        return true;
    return false;
}

void insert_border_path(struct BorderPath **head, int left, int right){
    struct BorderPath *new_border_path = malloc_BorderPath(left, right);
    if(*head == NULL){
        *head = new_border_path;
        return;
    }
    (*head)->prev = new_border_path;
    new_border_path->next = *head;
    *head = new_border_path;
}

void add_to_paths(struct BorderPath **head, int left, int right){
    struct BorderPath *i = *head;
    struct BorderPath *right_path = NULL;
    struct BorderPath *left_path = NULL;
    while(i != NULL){
        if(i->first_puzzle->ind == right)
            right_path = i;
        if(i->last_puzzle->ind == left)
            left_path = i;
        i = i->next;
    }
    if(right_path == NULL && left_path == NULL){
        insert_border_path(head, left, right);
        return;
    }
    if(right_path != NULL && left_path != NULL){
        left_path->len += right_path->len;
        left_path->last_puzzle->next = right_path->first_puzzle;
        left_path->last_puzzle = right_path->last_puzzle;
        delete_border_path(right_path, head);
        return;
    }
    if(right_path != NULL){
        right_path->len += 1;
        struct PuzzleNode *new_puzzle_node = malloc_Puzzlenode(left, right_path->first_puzzle);
        right_path->first_puzzle = new_puzzle_node;
        return;
    }
    else{
        left_path->len += 1;
        struct PuzzleNode *new_puzzle_node = malloc_Puzzlenode(right, NULL);
        left_path->last_puzzle->next = new_puzzle_node;
        left_path->last_puzzle = new_puzzle_node;
        return;
    }
}

void get_sides(struct Puzzle *p, int *s_left, int *s_right){
    int first_border = -1;
    for(int i=0; i<4; i++)
        if(p->sides[i].type == 0){
            first_border = i;
            break;
        }
    int next = (first_border+1)%4;
    if(p->sides[next].type != 0)
        *s_left = next;
    else
        *s_left = (next+1)%4;

    int prev = (3+first_border)%4;
    if(p->sides[prev].type != 0)
        *s_right = prev;
    else
        *s_right = (first_border+2)%4;
}

void first_two_available(int n, bool *available, int *indexes, int *first, int *second){
    int ind0=-1;
    int ind1=-1;
    for(int i=0; i<n; i++){
        if(available[indexes[i]]){
            if(ind0 == -1)
                ind0 = i;
            else{
                ind1 = i;
                break;
            }
        }
    }
    *first = ind0;
    *second = ind1;
}

void print_paths(struct BorderPath *paths, int *borders){
    int cnt = 1;
    if(paths->next == NULL){
        printf("Ramkę tworzą puzzle w kolejności:\n");
        struct PuzzleNode *p = paths->first_puzzle;
        while(p != NULL){
            printf("%d ", borders[p->ind] + 1);
            p = p->next;
        }
        printf("\n");
        return;
    }
    
    while(paths != NULL){
        printf("Kawalek nr %d\n", cnt);
        struct PuzzleNode *p = paths->first_puzzle;
        while(p != NULL){
            printf("%d ", borders[p->ind] + 1);
            p = p->next;
        }
        printf("\n");
        paths = paths->next;
        cnt++;
    }
}

void solve_border(struct FinalStruct finalStruct, bool *is_border, int *corners){
    struct BitmapInfo *puzzle_solution = finalStruct.puzzleSolution;
    struct BitmapInfo *sideUpward = finalStruct.sideUpward;
    struct Puzzle *puzzles = finalStruct.puzzles;
    int puzzle_width = puzzle_solution->width;
    int puzzle_height = puzzle_solution->height;

    int border_pieces = 2*(puzzle_width+puzzle_height)-4;
    int borders[border_pieces];
    int side_left[border_pieces];
    int side_right[border_pieces];
    int cnt = 0;
    for(int i=0; i<puzzle_height*puzzle_width; i++)
        if(is_border[i]){
            int s_left, s_right;
            get_sides(&puzzles[i], &s_left, &s_right);
            borders[cnt] = i;
            side_left[cnt] = s_left;
            side_right[cnt] = s_right;
            cnt++;
        }

    int matching_right_val[border_pieces][border_pieces];
    int matching_right_ind[border_pieces][border_pieces];
    int matching_left_val[border_pieces][border_pieces];
    int matching_left_ind[border_pieces][border_pieces];

    for(int i=0; i<border_pieces; i++){
        int puzzle_num = borders[i];
        int side_num = side_right[i];
        for(int j=0; j<border_pieces; j++){
            int puzzle2_num = borders[j];
            int side2_num = side_left[j];
            int res;
            if(i==j)
                res = NO_MATCH;
            else
                res = puzzle_match_val(&(puzzles[puzzle_num].sides[side_num]), &(puzzles[puzzle2_num].sides[side2_num]));
            matching_right_val[i][j] = res;
            matching_right_ind[i][j] = j;
            matching_left_val[j][i] = res;
            matching_left_ind[j][i] = i;
        }
    }

    for(int i=0; i<border_pieces; i++){
        select_sort_two(border_pieces, matching_right_val[i], matching_right_ind[i]);
        select_sort_two(border_pieces, matching_left_val[i], matching_left_ind[i]);
    }

    bool right_side_available[border_pieces];
    bool left_side_available[border_pieces];
    for(int i=0; i<border_pieces; i++){
        right_side_available[i] = true;
        left_side_available[i] = true;
    }

    struct BorderPath *paths = NULL;
    bool ready = false;
    while(!ready){
        ready = true;
        for(int i=0; i<border_pieces; i++){
            if(right_side_available[i] == false)
                continue;
            int ind0;
            int ind1;
            first_two_available(border_pieces, left_side_available, matching_right_ind[i], &ind0, &ind1);
            if(ind1 == -1){
                //ostatnia mozliwosc musi byc dobra
                right_side_available[i] = false;
                left_side_available[matching_right_ind[i][ind0]] = false;
                ready = true;
                break;
            }
            else if(great_match(matching_right_val[i][ind0], matching_right_val[i][ind1])){
                int puzzle2 = matching_right_ind[i][ind0];
                first_two_available(border_pieces, right_side_available, matching_left_ind[puzzle2], &ind0, &ind1);
                if(matching_left_ind[puzzle2][ind0] != i)
                    continue;
                if(great_match(matching_left_val[puzzle2][ind0], matching_left_val[puzzle2][ind1])){
                    //obustronny great match
                    //dodaj do paths
                    right_side_available[i] = false;
                    left_side_available[puzzle2] = false;
                    add_to_paths(&paths, i, puzzle2);
                    ready = false;
                }
            }
        }
    }

    for(int i=0; i<border_pieces; i++){
        if(left_side_available[i]){
            printf("Nie udalo sie ulozyc calej ramki\n");
            printf("Uzyskano nastepujace postepy:\n");
            print_paths(paths, borders);
            exit(0);
        }
    }
    print_paths(paths, borders);

    int solved_border[border_pieces];
    cnt = 0;
    struct PuzzleNode *p = paths->first_puzzle;
    while(p != NULL){
        solved_border[cnt] = borders[p->ind];
        p = p->next;
        cnt++;
    }

    int first_ind = -1;
    int second_ind = -1;
    for(int i=0; i<border_pieces; i++){
        int p = solved_border[i];
        if(is_in(p, corners)){
            if(first_ind==-1)
                first_ind = i;
            else{
                second_ind = i;
                break;
            }
        }
    }
    int side_len = second_ind-first_ind+1;
    if(side_len != puzzle_height && side_len != puzzle_width){
        printf("Blad podczas ukladania ramki\n");
        exit(0);
    }

    for(int i=0; i<puzzle_height; i++)
        for(int j=0; j<puzzle_width; j++)
            set_bitmap(puzzle_solution, j, i, 0);

    if(side_len == puzzle_width){
        for(int i=0; i<puzzle_width; i++){
            set_bitmap(puzzle_solution, i, 0, solved_border[first_ind+i]);
            set_bitmap(sideUpward, i, 0, (side_right[first_ind+i]+1)%4);
        }
        for(int i=1; i<puzzle_height; i++){
            set_bitmap(puzzle_solution, puzzle_width-1, i, solved_border[second_ind+i]);
            set_bitmap(sideUpward, puzzle_width-1, i, side_right[second_ind+i]);
        }
        int third_ind = second_ind+puzzle_height-1;
        if(!is_in(solved_border[third_ind], corners)){
            printf("Blad podczas ukladania ramki\n");
            exit(0);
        }
        for(int i=1; i<puzzle_width; i++){
            set_bitmap(puzzle_solution, puzzle_width-1-i, puzzle_height-1, solved_border[third_ind+i]);
            set_bitmap(sideUpward, puzzle_width-1-i, puzzle_height-1, (side_left[third_ind+i]+1)%4);
        }
        int fourth_ind = third_ind+puzzle_width-1;
        if(!is_in(solved_border[fourth_ind], corners)){
            printf("Blad podczas ukladania ramki\n");
            exit(0);
        }
        for(int i=1; i<puzzle_height-1; i++){
            set_bitmap(puzzle_solution, 0, puzzle_height-1-i, solved_border[(fourth_ind+i)%border_pieces]);
            set_bitmap(sideUpward, 0, puzzle_height-1-i, side_left[fourth_ind+i]);
        }
    }
    else{
        for(int i=0; i<puzzle_height; i++){
            set_bitmap(puzzle_solution, 0, i, solved_border[first_ind+i]);
            set_bitmap(sideUpward, 0, i, side_left[first_ind+i]);
        }
        for(int i=1; i<puzzle_width; i++){
            set_bitmap(puzzle_solution, i, puzzle_height-1, solved_border[second_ind+i]);
            set_bitmap(sideUpward, i, puzzle_height-1, (side_right[second_ind+i]+1)%4);
        }
        int third_ind = second_ind+puzzle_width-1;
        if(!is_in(solved_border[third_ind], corners)){
            printf("Blad podczas ukladania ramki\n");
            exit(0);
        }
        for(int i=1; i<puzzle_height; i++){
            set_bitmap(puzzle_solution, puzzle_width-1, puzzle_height-1-i, solved_border[third_ind+i]);
            set_bitmap(sideUpward, puzzle_width-1, puzzle_height-1-i, side_right[third_ind+i]);
        }
        int fourth_ind = third_ind+puzzle_height-1;
        if(!is_in(solved_border[fourth_ind], corners)){
            printf("Blad podczas ukladania ramki\n");
            exit(0);
        }
        for(int i=1; i<puzzle_width-1; i++){
            set_bitmap(puzzle_solution, puzzle_width-1-i, 0, solved_border[(fourth_ind+i)%border_pieces]);
            set_bitmap(sideUpward, puzzle_width-1-i, 0, (side_left[fourth_ind+i]+1)%4);
        }
    }
    return;
}
