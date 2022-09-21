#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void Is_Finfo(struct stat buf) {
    // [inodenumber, devicenumber, permission, user, fsize]
    // printf("[%ld %o %d")
}

int main(int argc, char *argv[]) {
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    char *cwd = (char *)malloc(sizeof(char) * 1024);
    memset(cwd, 0, 1024); // ???
    struct stat buf;

    getcwd(cwd, 1024);

    if ((dp = opendir(cwd)) == NULL) {
        printf("opendir() fail\n");
        exit(1);
    }

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &buf);
        if (S_ISREG(buf.st_mode)) // file
        {
            // print file info
            printf("%s\n", entry->d_name);
        }
    }

    free(cwd);
    closedir(dp);

    return 0;
}
