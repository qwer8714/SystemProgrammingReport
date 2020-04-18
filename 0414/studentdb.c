#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX 24
#define START_ID 1401001
#define ESC 27

int menu_select(void);
void dbCreate(char *argv[]);
void dbQuery(char *argv[]);
void dbUpdate(char *argv[]);

struct student {
    char name[MAX];
    int id;
    int score;
};

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "사용법 : %s file\n", argv[0]);
        exit(1);
    }

    int c;
    while ((c = menu_select()) != 0) {
        switch (c) {
            case 1:
                dbCreate(argv);
                break;
            case 2:
                dbQuery(argv);
                break;
            case 3:
                dbUpdate(argv);
                break;
            default:
                printf("\n올바른 숫자를 입력해주세요. \n\n");
                break;
        }
    }
    return 0;
}

int menu_select() {
    int select;

    printf("학생 DB \n\n");
    printf("1. DB 생성 \n");
    printf("2. DB 질의 \n");
    printf("3. DB 업데이트 \n");
    printf("4. DB 삭제 \n");
    printf("번호입력 : ");
    select = getchar() - 48;
    return select;
}

void dbCreate(char *argv[]){
    int fd;
    struct student rec;

    if ((fd = open(argv[1],O_WRONLY |O_CREAT, 0640))==-1) {
        perror(argv[1]);
        exit(2);
    }

    printf("%-9s %-8s %-4s \n", "학번", "이름", "점수");
    while (scanf("%d %s %d", &rec.id, rec.name, &rec.score) == 3) {
        lseek(fd, (rec.id - START_ID) * sizeof(rec), SEEK_SET);
        write(fd, &rec, sizeof(rec));

        char ch = getchar();
        if (ch == ESC || ch == EOF) break;
    }

    close(fd);
    exit(0);
}

void dbQuery(char *argv[]) {
    int fd, id;
    char c;
    struct student rec;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror(argv[1]);
        exit(2);
    }

    do {
        printf("\n검색할 학생의 학번 입력 : ");
        if (scanf("%d", &id) == 1 && id >= START_ID) {
            lseek(fd, (id - START_ID) * sizeof(rec), SEEK_SET);
            if ((read(fd, &rec, sizeof(rec)) > 0) && (rec.id != 0))
                printf("학번 : %d\t 이름 : %s\t 점수 : %d\n", rec.id, rec.name, rec.score);
            else
                printf("레코드 %d 없음\n", id);
        } else
            printf("입력 오류\n");

        printf("계속하시겠습니까? (Y/N)");
        scanf(" %c", &c);
    } while (c == 'Y');

    close(fd);
    exit(0);
}

void dbUpdate(char *argv[]) {
    int fd, id;
    char c;
    struct  student rec;

    if ((fd = open(argv[1], O_RDWR)) == -1) {
        perror(argv[1]);
        exit(2);
    }

    do {
        printf("수정할 학생의 학번 입력 : ");
        if (scanf("%d", &id) == 1 && id >= START_ID) {
            lseek(fd, (long) (id - START_ID) * sizeof(rec), SEEK_SET);
            if ((read(fd, &rec, sizeof(rec)) > 0) && (rec.id != 0)) {
                printf("학번 : %8d\t 이름 : %4s\t 점수 : %4d\n", rec.id, rec.name, rec.score);
                printf("새로운 점수 : ");
                scanf("%d", &rec.score);
                lseek(fd, (long) -sizeof(rec), SEEK_CUR);
                write(fd, &rec, sizeof(rec));
            } else
                printf("레코드 %d 없음\n", id);
        } else
            printf("입력오류\n");

        printf("계속하시겠습니까? (Y/N)");
        scanf(" %c", &c);
    } while (c == 'Y');

    close(fd);
    exit(0);
}
