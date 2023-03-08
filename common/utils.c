#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

int isDir(char* path) {
    struct stat st;
    lstat(path, &st);
    return S_ISDIR(st.st_mode);   
}

char* strjoin(char* st_1, char * st_2) {
    char* result = malloc(strlen(st_1) + strlen(st_2) + 2);
    strcpy(result, st_1);
    strcat(result, st_2);
    return result;
}