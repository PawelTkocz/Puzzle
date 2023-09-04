#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#define BLACK_MAX 90
#define WHITE_MIN 230
#define GREEN_RED_DIFF 20

void ppm_header(FILE* ptr, int width, int height){
    fprintf(ptr, "%s\n%d %d\n255\n", "P3" ,width, height);
}

bool puzzle_pixel(int r, int g, int b){
    if(r<BLACK_MAX && g<BLACK_MAX && b<BLACK_MAX)
        return false;
    if(abs(g-r)>GREEN_RED_DIFF)
        return false;
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
