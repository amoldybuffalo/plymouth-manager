#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "../common/utils.h"

void install(char* theme) {
    FILE* cmd;
    char cmd_output[PATH_MAX];

    char* pm_theme_dirname = strjoin("/usr/share/plymouth-manager/themes/", theme);

    char* plymouth_theme_dirname = strjoin("/usr/share/plymouth/themes/", theme);

    printf("Copying theme...\n");
    fflush(stdout);
    DIR* plymouth_theme_dir = opendir(plymouth_theme_dirname);
    if(ENOENT == errno){ 
        //If the directory does not exist already, copy it to the plymouth theme folder
        char* copy_theme_cmd = malloc(3000);
        sprintf(copy_theme_cmd, "cp -R %s %s", pm_theme_dirname, plymouth_theme_dirname);
        cmd = popen(copy_theme_cmd, "r"); 
        while(fgets(cmd_output, PATH_MAX, cmd) != NULL); //Forces the program to wait until the command has finished.    
        pclose(cmd);
        
    }  
    closedir(plymouth_theme_dir);
    
    printf("Updating settings...\n");
    fflush(stdout);
    char* update_settings_cmd = malloc (1000);
    sprintf(update_settings_cmd, "cp %s/%s.plymouth /usr/share/plymouth/themes/default.plymouth", pm_theme_dirname, theme);
    cmd = popen(update_settings_cmd, "r");
    while(fgets(cmd_output, PATH_MAX, cmd) != NULL); //Forces the program to wait until the command has finished.    
    pclose(cmd);
    
    printf("Updating the initramfs...\n");
    fflush(stdout);
    cmd = popen("update-initramfs -u", "r");
    while(fgets(cmd_output, PATH_MAX, cmd) != NULL); //Forces the program to wait until the command has finished.    
    pclose(cmd);
    printf("Done.\n");
    fflush(stdout);
    
}

void list_themes() {
    DIR* theme_dir = opendir("/usr/share/plymouth-manager/themes/");
    struct dirent* entry;
    if(theme_dir) {
        while((entry = readdir(theme_dir)) != NULL) { 
            char* path = strjoin("/usr/share/plymouth-manager/themes/", entry->d_name);
            if(isDir(path) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("%s\n", entry->d_name);
            }
        }   
    }
}

void parse_arguments(int argc, char** argv) {
    if(argc < 2) return;
    if(strcmp(argv[1], "install") == 0) {
        if(getuid() != 0) {
            printf("Error, must be running as root to install theme.");
            exit(1);
        } else {
            if( argc >= 3) {
                install(argv[2]);
            } else {
                printf("Please provide an argument for 'theme'");
            }
            
        }

    } else if (strcmp(argv[1],"list") == 0) {
        list_themes();
    }
}


int main(int argc, char** argv) {
    parse_arguments(argc, argv);
}