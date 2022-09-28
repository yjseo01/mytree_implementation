#ifndef _HEADER_H_
#define _HEADER_H_
#endif

#include <dirent.h>
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

/* 파일의 세부 정보를 저장하는 함수 */
void SetInfo(fileinfo *info, char *path, char *name);

/* 파일의 세부 정보 및 트리 구조를 출력하는 함수 */
void PrintTree(const fileinfo *info, int *flr, const int dcnt, const int fcnt,
               const int idx);

/* 인자로 주어진 파일의 경로를 반환하는 함수 */
char *MakePath(const char *path, const char *dirname);

/* 디렉토리 내의 파일을 탐색 및 출력하는 함수 */
void Search(const char *dpath, int *floor, int *tdir, int *tfile, int *f_idx);
