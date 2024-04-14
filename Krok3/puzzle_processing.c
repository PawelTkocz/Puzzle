#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>

void fill_infile_name(char *fname, int i){
    strcpy(fname, "Puzzles/p");
    char snum[5];
    sprintf(snum, "%d", i);
    strcat(fname, snum);
}

void fill_outfile_name(char *fname, int i){
    getcwd(fname, PATH_MAX);
    char a = '\\';
    strncat(fname, &a, 1);
    strcat(fname, "Puzzle_result\\p");
    char snum[5];
    sprintf(snum, "%d", i);
    strcat(fname, snum);
    strcat(fname, "_ok.ppm");
}

void ppm_header(FILE* ptr, int width, int height){
    fprintf(ptr, "%s\n%d %d\n255\n", "P3" ,width, height);
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
