#include <stdio.h>
#include <stdlib.h>


//tez mozna dodac watki

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
    if(res2 > 2*res1)
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

void solve_border(int *puzzle_solution, int puzzle_width, int puzzle_height, bool *is_border, int *corners, struct Puzzle *puzzles){

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

    for(int i=0; i<border_pieces; i++){
        for(int j=0; j<border_pieces; j++)
            printf("%d ", matching_right_val[i][j]);
        printf("\n\n\n");
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
                left_side_available[ind0] = false;
                add_to_paths(&paths, i, ind0);
                ready = false;
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

    int no_matches_left = 0;
    int no_matches_right = 0;
    for(int i=0; i<border_pieces; i++){
        if(left_side_available[i])
            no_matches_left++;
        if(right_side_available[i])
            no_matches_right++;
    }
    printf("%d %d\n", no_matches_left, no_matches_right);

    cnt = 1;
    while(paths != NULL){
        printf("%d. %d\n", cnt, paths->len);
        paths = paths->next;
    }
    /*
    int matching[border_pieces-1];
    int puzzel_number[border_pieces-1];

    printf("pierwsza ramka %d %d\n", borders[1], side_right[1]);
    for(int i=0; i<border_pieces; i++){
        int puzzle_num = borders[i];
        int side_num = side_left[i];
        double res = puzzle_match_val(&(puzzles[borders[1]].sides[side_right[1]]), &(puzzles[puzzle_num].sides[side_num]));
        matching[i] = res;
        puzzel_number[i] = puzzle_num;
    }

    select_sort_two(border_pieces, matching, puzzel_number);
    for(int i=0; i<border_pieces; i++){
        printf("%d %d\n", matching[i], puzzel_number[i]);
    }
    */
    return;
}
