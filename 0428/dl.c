#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char type(mode_t);
char *perm(mode_t);
void printStat(char*, char*, struct stat*);

/* 디렉터리 내의 파일 이름들을 리스트한다. */
int main(int argc, char **argv) {
    DIR *dp;
    char *dir;
    struct dirent *d;
    struct stat st;
    char path[BUFSIZ+1];

    dir = ".";  // 현재 디렉터리를 대상으로

    if ((dp = opendir(dir)) == NULL) {  // 디렉터리 열기
        perror(dir);
        exit(1);
    }

    if (argc == 1) { // 옵션 추가가 없을때
        while ((d = readdir(dp)) != 0) {  // 디렉터리 내의 각 엔트리에 대해
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) // . ..가 존재 할시 continue
                continue;
            printf("%s \n", d->d_name); // 파일 이름 프린트
        }
    } else if (strcmp("-a", argv[1]) == 0) {    // a 옵션
        while ((d = readdir(dp)) != 0) {  // 디렉터리 내의 각 엔트리에 대해
            printf("%s \n", d->d_name);    // 파일 이름 프린트
        }
    } else if (strchr(argv[1], 'l') != 0) {	// l 옵션이 포함 되어있는지 체크
        while ((d = readdir(dp)) != 0) { // 디렉토리 내의 각 파일에 대해
            sprintf(path, "%s/%s", dir, d->d_name); // 파일 경로명 만들기
            if (lstat(path, &st) < 0)   // 파일 상태 정보 가져오기
                perror(path);
            else {
                if (strchr(argv[1], 'a') == 0) // al 옵션이 아닌 l 옵션
                    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) // . ..가 존재 할시 continue
                        continue;
                printStat(path, d->d_name, &st);  // 상태 정보 출력
            }
        }
    } else { // 다른 입력이 들어왔을 시 사용가능한 옵션 목록 출력
        printf("%s\n", "-a, -l, -al 옵션 사용가능합니다.");
    }

    closedir(dp);
    exit(0);
}
/* 파일 상태 정보를 출력 */
void printStat(char *pathname, char *file, struct stat *st) {

    printf("%c%s ", type(st->st_mode), perm(st->st_mode));
    printf("%3ld ", st->st_nlink);
    printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
    printf("%9ld ", st->st_size);
    printf("%.12s ", ctime(&st->st_mtime)+4);
    printf("%s\n", file);
}

/* 파일 타입을 리턴 */
char type(mode_t mode) {

    if (S_ISREG(mode))
        return('-');
    if (S_ISDIR(mode))
        return('d');
    if (S_ISCHR(mode))
        return('c');
    if (S_ISBLK(mode))
        return('b');
    if (S_ISLNK(mode))
        return('l');
    if (S_ISFIFO(mode))
        return('p');
    if (S_ISSOCK(mode))
        return('s');
}

/* 파일 허가권을 리턴 */
char* perm(mode_t mode) {
    int i;
    static char perms[10];

    strcpy(perms, "---------");

    for (i=0; i < 3; i++) {
        if (mode & (S_IREAD >> i*3))
            perms[i*3] = 'r';
        if (mode & (S_IWRITE >> i*3))
            perms[i*3+1] = 'w';
        if (mode & (S_IEXEC >> i*3))
            perms[i*3+2] = 'x';
    }
   return(perms);
}

/*
   시스템 프로그래밍 수업에 사용한 코드 사용하여 ls명령어 옵션 -a, -l 추가
*/
