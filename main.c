#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// inodenumber, divnumver, permission, user, filesize, file name
typedef struct info_set { // 파일 세부정보 저장
    int inonum;           // inode number
    unsigned long devnum; /* device */
    char permission[11];  // 권한
    char *UID;            // 사용자 ID
    long size;            // 파일 크기
    char *fname;          // 파일 이름
} info_set;

void SetInfo(info_set *info, char *path, char *name) {
    struct stat buf;
    struct passwd *pwd;

    if (lstat(path, &buf) == -1) {
        perror("Error: ");
        exit(1);
    }

    if (S_ISDIR(buf.st_mode)) 
        info->permission[0] = 'd';
    else if (S_ISCHR(buf.st_mode))
        info->permission[0] = 'c';
    else if (S_ISBLK(buf.st_mode))
        info->permission[0] = 'b';
    else if (S_ISFIFO(buf.st_mode))
        info->permission[0] = 'p';
    else if (S_ISSOCK(buf.st_mode))
        info->permission[0] = 's';
    else if (S_ISLNK(buf.st_mode))
        info->permission[0] = 'l';
    else
        info->permission[0] = '-';

    info->permission[1] =
        ((buf.st_mode & S_IRUSR) != 0 ? 'r' : '-'); 
    info->permission[2] = ((buf.st_mode & S_IWUSR) != 0 ? 'w' : '-');
    info->permission[3] = ((buf.st_mode & S_IXUSR) != 0 ? 'x' : '-');
    if (buf.st_mode & S_ISUID)
        info->permission[3] = 'S';

    info->permission[4] = ((buf.st_mode & S_IRGRP) != 0 ? 'r' : '-');
    info->permission[5] = ((buf.st_mode & S_IWGRP) != 0 ? 'w' : '-');
    info->permission[6] = ((buf.st_mode & S_IXGRP) != 0 ? 'x' : '-');
    if (buf.st_mode & S_ISGID)
        info->permission[6] = 'S';

    info->permission[7] = ((buf.st_mode & S_IROTH) != 0 ? 'r' : '-');
    info->permission[8] = ((buf.st_mode & S_IWOTH) != 0 ? 'w' : '-');
    info->permission[9] = ((buf.st_mode & S_IXOTH) != 0 ? 'x' : '-');
    if (buf.st_mode & S_ISVTX)
        info->permission[9] = 'T';
    info->permission[10] = '\0';

    pwd = getpwuid(buf.st_uid);

    info->UID = pwd->pw_name; 
    info->fname = name;
    info->size = buf.st_size;
    info->devnum = buf.st_dev;
    info->inonum = buf.st_ino;
}

void Is_Finfo(const info_set *info, int *flr, const int dcnt, const int fcnt,
              const int idx) {
    for (int i = 0; i < idx; i++) {
        flr[i] == 0 ? printf("│   ") : printf("    ");
    }
    fcnt == 0 && dcnt == 0 ? printf("└── ") : printf("├── ");
    printf("[ %d %ld %s %s     %ld]   ", info->inonum, info->devnum,
           info->permission, info->UID, info->size);
    printf("%s\n", info->fname);
}

char *MakePath(const char *path, const char *dirname) {
    char *pathnull = (char *)malloc(sizeof(char) * 1024 + sizeof(path));
    strcpy(pathnull, path);
    strcat(pathnull, "/");
    strcat(pathnull, dirname);
    return pathnull;
}

void Tree(const char *dpath, int *floor, int *tdir, int *tfile, int *f_idx) {
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    struct stat buf;

    info_set *dirs = (info_set *)malloc(sizeof(info_set) * 1024);
    info_set *files = (info_set *)malloc(sizeof(info_set) * 1024);

    int d_cnt = 0;
    int f_cnt = 0;

    if ((dp = opendir(dpath)) == NULL) {
        printf("opendir() fail\n");
        exit(1);
    }

    while ((entry = readdir(dp)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char *path = MakePath(dpath, entry->d_name);

        info_set *info_buf = NULL;

        if (lstat(path, &buf) == -1) {
            printf("%s lstat() fail\n", entry->d_name);
            exit(1);
        }

        if (S_ISREG(buf.st_mode)) { // file
            info_buf = &files[f_cnt];
            f_cnt++;
        } else if (S_ISDIR(buf.st_mode)) { // dir
            info_buf = &dirs[d_cnt];
            d_cnt++;
        }

        SetInfo(info_buf, path, entry->d_name);

        free(path);
    }

    for (int j = 0; j < f_cnt; j++) { // file
        Is_Finfo(&(files[j]), floor, d_cnt, f_cnt - j - 1, *f_idx);
    }

    for (int i = 0; i < d_cnt; i++) { // dir
        Is_Finfo(&(dirs[i]), floor, d_cnt - i - 1, 0, *f_idx);
        if (d_cnt - i - 1 == 0) {
            floor[*f_idx] = 1;
        }
        (*f_idx)++;
        char *newdirpath = MakePath(dpath, dirs[i].fname);
        *tdir += d_cnt;
        *tfile += f_cnt;
        Tree(newdirpath, floor, tdir, tfile, f_idx);

        free(newdirpath);
        floor[*f_idx] = 0;
        (*f_idx)--;
    }

    free(files);
    free(dirs);

    closedir(dp);
    return;
}

int main(int argc, char *argv[]) {
    char *cwd = (char *)malloc(sizeof(char) * 1024);
    getcwd(cwd, 1024);

    int *floor = (int *)malloc(sizeof(int) * 1024);
    int *f_idx = NULL;

    memset(floor, 0, 1024);
    int total_dir = 0;
    int total_file = 0;

    int idx = 0;
    f_idx = &idx;

    Tree(cwd, floor, &total_dir, &total_file, f_idx);
    printf("\n%d directories, %d files\n", total_dir, total_file);

    free(floor);
    free(cwd);
    return 0;
}
