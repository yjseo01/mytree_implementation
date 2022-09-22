#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void Is_Finfo(const char *filename, const int flr, const int dcnt) {
    // [inodenumber, devicenumber, permission, user, fsize]
    // printf("[%ld %o %d")
    struct stat fbuf;
    lstat(filename, &fbuf);
    for (int i = 0; i < flr; i++) {
        printf("   ");
    }
    dcnt == 0 ? printf("┗━ ") : printf("┠━ ");
    printf("[file info]   ");
    printf("%s\n", filename);
}

void Tree(const char *dname, int floor) {
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    struct stat buf;

    char **dirs = (char **)malloc(sizeof(char *) * 1024);
    char **files = (char **)malloc(sizeof(char *) * 1024);
    int d_cnt = 0;
    int f_cnt = 0;

    if ((dp = opendir(dname)) == NULL) {
        printf("opendir() fail\n");
        exit(1);
    }

    while ((entry = readdir(dp)) != NULL) {
        if (lstat(entry->d_name, &buf) == -1) {
            printf("%s lstat() fail\n", entry->d_name);
            exit(1);
        }
        if (S_ISREG(buf.st_mode)) {
            files[f_cnt] = entry->d_name;
            f_cnt++;
        } else if (S_ISDIR(buf.st_mode)) {
            dirs[d_cnt] = entry->d_name;
            d_cnt++;
        }
    }

    for (int j = 0; j < f_cnt; j++) { // file
        Is_Finfo(files[j], floor, d_cnt);
    }

    if (d_cnt == 0) {
        free(files);
        free(dirs);
        closedir(dp);
        return;
    }

    for (int i = 0; i < d_cnt; i++) { // dir
        Is_Finfo(dirs[i], floor, d_cnt - i - 1);
        floor++;
        Tree(dirs[i], floor);
        floor--;
    }

    free(files);
    free(dirs);
    closedir(dp);
    return;
}

int main(int argc, char *argv[]) {
    char *cwd = (char *)malloc(sizeof(char) * 1024);
    getcwd(cwd, 1024);

    int floor = 0;

    Tree(cwd, floor);

    free(cwd);
    return 0;
}
