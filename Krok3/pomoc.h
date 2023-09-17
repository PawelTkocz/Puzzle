#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#define BLACK_MAX 80
#define WHITE_MIN 125
#define GREEN_RED_DIFF 20

void select_sort_two(int n, int *tab, int *tab2){
    for(int i=0; i<n-1; i++){
        int min_ind = i;
        int min_val = tab[i];
        for(int j=i+1; j<n; j++){
            if(tab[j] < min_val){
                min_val = tab[j];
                min_ind = j;
            }
        }
        int temp = tab[i];
        tab[i] = tab[min_ind];
        tab[min_ind] = temp;

        int temp2 = tab2[i];
        tab2[i] = tab2[min_ind];
        tab2[min_ind] = temp2;
    }
}

void find_line(int x1, int y1, int x2, int y2, double *a, double *b, double *c){
    *a=1.0;
    *b=0.0;
    *c=-1.0*(double)x1;
    if(x1!=x2){
        *a = (double)(y2-y1)/(x1-x2);
        *b = 1.0;
        *c = -1.0*(double)((*a)*x1+y1);
    }
}

void fill_infile_name(char *fname, int i){
    strcpy(fname, "Nowe_puzzle/p");
    char snum[5];
    itoa(i, snum, 10);
    strcat(fname, snum);
}

void fill_outfile_name(char *fname, int i){
    getcwd(fname, PATH_MAX);
    char a = '\\';
    strncat(fname, &a, 1);
    strcat(fname, "Nowe_puzzle_res\\p");
    char snum[5];
    itoa(i, snum, 10);
    strcat(fname, snum);
    strcat(fname, "_ok.ppm");
}

void parallel_line_point(double a, double b, double c, int x, int y, double *a_res, double *b_res, double *c_res){
    *a_res = a;
    *b_res = b;
    *c_res = -1*a*x - b*y;
}

void ppm_header(FILE* ptr, int width, int height){
    fprintf(ptr, "%s\n%d %d\n255\n", "P3" ,width, height);
}

double point_dist(int x1, int y1, int x2, int y2){
    int x = abs(x1-x2);
    int y = abs(y1-y2);
    return sqrt(x*x + y*y);
}

void find_prostopadly_vector(double a, double b, double c, double *x, double *y, double len){
    double len_temp = sqrt(a*a + b*b);
    double f = len/len_temp;
    *x = a*f;
    *y = b*f;
}

bool puzzle_pixel(int r, int g, int b){
    if(r<BLACK_MAX && g<BLACK_MAX && b<BLACK_MAX)
        return false;
    //if(abs(g-r)>GREEN_RED_DIFF)
    //    return false;
    if(r>WHITE_MIN && g>WHITE_MIN && b>WHITE_MIN)
        return false;
    return true;
}

int get_directory_files(char *dir_name){
    char dir_path[PATH_MAX];
    getcwd(dir_path, sizeof(dir_path));
    char a = '\\';
    strncat(dir_path, &a, 1);
    strcat(dir_path, dir_name);

    DIR* FD = opendir(dir_path);
    if(FD == NULL){
        printf("Nie udalo sie otworzyc folderu o nazwie %s\n", dir_name);
        return 1;
    }

    struct dirent* in_file;
    while((in_file = readdir(FD))){
        if(!strcmp(in_file->d_name, "."))
            continue;
        if(!strcmp(in_file->d_name, ".."))
            continue;
        printf("%s\n", in_file->d_name);
    }
    closedir(FD);
    return 0;
}
