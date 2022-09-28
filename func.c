

#include "header.h"

void SetInfo(fileinfo *info, char *path, char *name) {
    struct stat buf;
    struct passwd *pwd;

    if (lstat(path, &buf) == -1) {
        perror("lsat() error: ");
        exit(1);
    }

    // 권한
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

    info->permission[1] = ((buf.st_mode & S_IRUSR) != 0 ? 'r' : '-');
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

    info->UID = pwd->pw_name;  // 소유자ID
    info->fname = name;        // 파일 이름
    info->size = buf.st_size;  // 파일 크기
    info->devnum = buf.st_dev; // device number
    info->inonum = buf.st_ino; // indoe number
}

void PrintTree(const fileinfo *info, int *flr, const int dcnt, const int fcnt,
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

void Search(const char *dpath, int *floor, int *tdir, int *tfile, int *f_idx) {
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    struct stat buf;

    fileinfo *dirs = (fileinfo *)malloc(sizeof(fileinfo) * MAX_FILE_NUM);
    fileinfo *files = (fileinfo *)malloc(sizeof(fileinfo) * MAX_FILE_NUM);

    int d_cnt = 0;
    int f_cnt = 0;

    if ((dp = opendir(dpath)) == NULL) {
        perror("opendir() error: ");
        exit(1);
    }

    while ((entry = readdir(dp)) != NULL) {

        // 숨김파일이면 출력하지 않고 넘어감
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char *path = MakePath(dpath, entry->d_name);

        fileinfo *info_buf = NULL;

        if (lstat(path, &buf) == -1) {
            perror("lstat() error: ");
            exit(1);
        }

        // 파일과 디렉토리를 각각 구분해서 배열에 저장
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

    // 트리 구조 출력
    for (int j = 0; j < f_cnt; j++) { // file
        PrintTree(&(files[j]), floor, d_cnt, f_cnt - j - 1, *f_idx);
    }

    free(files);

    for (int i = 0; i < d_cnt; i++) { // dir
        PrintTree(&(dirs[i]), floor, d_cnt - i - 1, 0, *f_idx);
        if (d_cnt - i - 1 == 0)
            floor[*f_idx] = 1;
        (*f_idx)++;

        char *newdirpath = MakePath(dpath, dirs[i].fname);

        // 하단에 출력될 파일과 디렉토리 총 개수 업데이트
        *tdir += d_cnt;
        *tfile += f_cnt;

        Search(newdirpath, floor, tdir, tfile, f_idx);

        free(newdirpath);
        floor[*f_idx] = 0;
        (*f_idx)--;
    }

    free(dirs);

    closedir(dp);
    return;
}
