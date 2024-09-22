#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000  // 한 줄에서 읽을 최대 문자 수

int compare_files(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");

    if (fp1 == NULL || fp2 == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }

    char line1[MAX_LINE_LENGTH], line2[MAX_LINE_LENGTH];
    int line_number = 1;

    // 두 파일을 한 줄씩 읽어서 비교
    while (fgets(line1, MAX_LINE_LENGTH, fp1) != NULL && fgets(line2, MAX_LINE_LENGTH, fp2) != NULL) {
        // 줄의 내용이 다르면 다르다고 출력
        if (strcmp(line1, line2) != 0) {
            printf("파일 내용이 일치하지 않습니다 (라인 %d).\n", line_number);
            printf("파일 1: %s파일 2: %s", line1, line2);
            fclose(fp1);
            fclose(fp2);
            return 0;
        }
        line_number++;
    }

    // 한 파일은 끝났는데 다른 파일은 아직 내용이 남아있는 경우
    if ((fgets(line1, MAX_LINE_LENGTH, fp1) != NULL) || (fgets(line2, MAX_LINE_LENGTH, fp2) != NULL)) {
        printf("파일의 길이가 다릅니다.\n");
        fclose(fp1);
        fclose(fp2);
        return 0;
    }

    printf("파일 내용이 일치합니다.\n");

    fclose(fp1);
    fclose(fp2);
    return 0;
}

int main() {
    char file1[100], file2[100];

    printf("첫 번째 파일 이름을 입력하세요: ");
    scanf("%s", file1);

    printf("두 번째 파일 이름을 입력하세요: ");
    scanf("%s", file2);

    compare_files(file1, file2);

    return 0;
}