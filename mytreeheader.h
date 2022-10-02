#include <dirent.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_FILE_NUM 1024

/* 파일의 세부정보 저장을 위한 구조체 */
struct Fileinfo {
    int inonum;           // inode number
    unsigned long devnum; // device number
    char permission[11];  // 권한
    char *UID;            // 사용자 ID
    long size;            // 파일 크기
    char *fname;          // 파일 이름
} typedef fileinfo;

/* fileinfo 구조체에 정보를 저장하는 함수 */
void SetInfo(fileinfo *info, char *path, char *name);

/* 파일의 크기를 human readable로 바꿔서 반환하는 함수 */
char *hoption(long size);

/* 트리 구조 및 파일 정보를 출력하는 함수 */
void PrintTree(const fileinfo *info, int *flr, const int cnt, const int idx);

/* fileinfo 구조체 f1을 f2로 복사하는 함수 */
void fcpy(fileinfo *f2, const fileinfo *f1);

/* fileinfo 구조체 배열을 알파벳 순서대로 정렬하는 함수 */
void *Sort(fileinfo set[1024], const int cnt);

/* 파일의 이름을 입력하면 해당 파일의 경로를 반환하는 함수 */
char *MakePath(const char *path, const char *dirname);

/* 디렉토리 내의 파일을 탐색하는 함수 */
void Search(const char *dpath, int *flr, int *tdir, int *tfile, int *f_idx);
