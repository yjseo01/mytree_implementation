#include "mytreeheader.h"

int main(int argc, char *argv[]) {
    char *cwd = (char *)malloc(sizeof(char) * MAX_FILE_NUM);
    getcwd(cwd, MAX_FILE_NUM);

    int *flr = (int *)malloc(sizeof(int) * MAX_FILE_NUM);
    int *f_idx = NULL;

    memset(flr, 0, MAX_FILE_NUM);

    int total_dir = 0;
    int total_file = 0;

    int idx = 0;
    f_idx = &idx;

    printf("\n");
    Search(cwd, flr, &total_dir, &total_file, f_idx);
    printf("\n%d directories, %d files\n", total_dir, total_file);

    free(flr);
    free(cwd);
    return 0;
}
