#include "header.h"
int main(int argc, char *argv[]) {
    char *cwd = (char *)malloc(sizeof(char) * MAX_FILE_NUM);
    getcwd(cwd, MAX_FILE_NUM);

    int *floor = (int *)malloc(sizeof(int) * MAX_FILE_NUM);
    int *f_idx = NULL;

    memset(floor, 0, MAX_FILE_NUM);

    int total_dir = 0;
    int total_file = 0;

    int idx = 0;
    f_idx = &idx;

    Search(cwd, floor, &total_dir, &total_file, f_idx);
    printf("\n%d directories, %d files\n", total_dir, total_file);

    free(floor);
    free(cwd);
    return 0;
}
