#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

int isDir(char* path) {
    struct stat st;
    lstat(path, &st);
    return S_ISDIR(st.st_mode);   
}

char* strjoin(char* st_1, char * st_2) {
    char* result = malloc(strlen(st_1) + strlen(st_2) + 2);
    sprintf(result, "%s%s", st_1, st_2);
    return result;
}

int file_exists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    int is_exist = 0;
    if (fp != NULL)
    {
        is_exist = 1;
        fclose(fp); // close the file
    }
    return is_exist;
}

//This assumes numbers are contiguous i.e. there are no breaks between start and the highest number.
int find_highest_file_number(char* path_start, int starting_point, char* path_end) {
    int i = starting_point;
    char curr_path[PATH_MAX];
    while(1) {
        sprintf(curr_path, "%s%i%s", path_start, i, path_end);  
        if(file_exists(curr_path)) {
            i++;
        } else {
            if(i == starting_point) return -1;
            return i - 1;
        }
    }
}