#include "mytreeheader.h"

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

char *hoption(long size) {
    if (size == 0)
        return "0";

    static char hsize[10];

    if (size < 1000)
        sprintf(hsize, "%ld", size);

    else if (size / 1000.0 < 1000) {
        double ssize = size / 1000.0;
        if (ssize >= 10)
            sprintf(hsize, "%dK", (int)ssize);
        else
            sprintf(hsize, "%.1fK", floor(ssize * 10) / 10);
    }

    else if (size / 1000000.0 < 1000) {
        double ssize = size / 1000000.0;
        if (ssize >= 10)
            sprintf(hsize, "%dM", (int)ssize);
        else
            sprintf(hsize, "%.1fM", floor(ssize * 10) / 10);
    }

    else if (size / 1000000000.0 < 1000) {
        double ssize = size / 1000000000.0;
        if (ssize >= 10)
            sprintf(hsize, "%dG", (int)ssize);
        else
            sprintf(hsize, "%.1fG", floor(ssize * 10) / 10);
    }

    return hsize;
}

void PrintTree(const fileinfo *info, int *flr, const int cnt, const int idx) {
    for (int i = 0; i < idx; i++) {
        flr[i] == 0 ? printf("│   ") : printf("    ");
    }
    cnt == 0 ? printf("└── ") : printf("├── ");
    printf("[ %d %ld %s %s %7s]  ", info->inonum, info->devnum,
           info->permission, info->UID, hoption(info->size));
    printf("%s\n", info->fname);
}

void fcpy(fileinfo *f2, const fileinfo *f1) {
    f2->devnum = f1->devnum;
    f2->inonum = f1->inonum;
    f2->size = f1->size;
    f2->fname = strdup(f1->fname);

    for (int i = 0; i < 11; i++)
        f2->permission[i] = f1->permission[i];

    f2->UID = strdup(f1->UID);
}

void *Sort(fileinfo set[1024], const int cnt) {
    fileinfo temp;
    int min;

    for (int i = 0; i < cnt - 1; i++) {
        min = i;
        for (int j = i + 1; j < cnt; j++) {
	    char *s1 = malloc(sizeof(char) * strlen(set[min].fname));
	    char *s2 = malloc(sizeof(char) * strlen(set[j].fname));

	    int k = 0;
	    int l = 0;

	    for (k; set[min].fname[k]; k++) {
	        if (set[min].fname[k] >= 65 && set[min].fname[k] <= 90)
		    s1[k] = set[min].fname[k] + 32;
		else
		    s1[k] = set[min].fname[k];
	    }

	    for (l; set[j].fname[l]; l++) {
	        if (set[j].fname[l] >= 65 && set[j].fname[l] <= 90)
		    s2[l] = set[j].fname[l] + 32;
		else
		    s2[l] = set[j].fname[l];
	    }

            if (strcmp(s1, s2) > 0) {
                min = j;
            }
	    
	    free(s1);
	    free(s2);
        }
        fcpy(&temp, &(set[i]));
        fcpy(&(set[i]), &(set[min]));
        fcpy(&(set[min]), &temp);
    }
}

char *MakePath(const char *path, const char *dirname) {
    char *pathnull = (char *)malloc(sizeof(char) * 1024 + sizeof(path));
    strcpy(pathnull, path);
    strcat(pathnull, "/");
    strcat(pathnull, dirname);
    return pathnull;
}

void Search(const char *dpath, int *flr, int *tdir, int *tfile, int *f_idx) {
    DIR *dp = NULL;
    struct dirent *entry = NULL;
    struct stat buf;

    fileinfo *files = (fileinfo *)malloc(sizeof(fileinfo) * MAX_FILE_NUM);

    int cnt = 0;

    if ((dp = opendir(dpath)) == NULL) {
        perror("opendir() error: ");
        exit(1);
    }

    while ((entry = readdir(dp)) != NULL) {

        // 숨김파일이면 출력하지 않고 넘어감
	if (entry->d_name[0] == '.') continue;

        char *path = MakePath(dpath, entry->d_name);

        fileinfo *info_buf = NULL;

        if (lstat(path, &buf) == -1) {
            perror("lstat() error: ");
            exit(1);
        }

        info_buf = &files[cnt];

        if (S_ISREG(buf.st_mode))
            (*tfile)++;
        else if (S_ISDIR(buf.st_mode))
            (*tdir)++;

        cnt++;

        SetInfo(info_buf, path, entry->d_name);

        free(path);
    }

    Sort(files, cnt);

    // 트리 구조 출력
    for (int i = 0; i < cnt; i++) {
        PrintTree(&(files[i]), flr, cnt - i - 1, *f_idx);

        if (files[i].permission[0] == 'd') // 디렉토리
        {
            if (cnt - i - 1 == 0) {
                flr[*f_idx] = 1;
            }
            (*f_idx)++;
            char *newdirpath = MakePath(dpath, files[i].fname);
            Search(newdirpath, flr, tdir, tfile, f_idx);

            free(newdirpath);
            flr[*f_idx] = 0;
            (*f_idx)--;
        }
    }

    free(files);
    closedir(dp);
    return;
}
