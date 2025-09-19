#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "student_ioctl.h"

int main(void)
{
    int fd = open("/dev/student", O_RDWR);
    if (fd < 0) {
        perror("open /dev/student");
        return 1;
    }

    struct student_ioctl s;

    /* 添加多条学生数据 */
    struct {
        int id;
        char name[16];
    } dataset[] = {
        {2023103111, "Alice"},
        {2022201456, "Bob"},
        {2023103122, "Carol"},
        {2022202457, "David"},
        {2024103113, "Eve"},
        {2023201789, "Frank"},
        {2023103555, "Grace"},
        {2023201333, "Heidi"},
        {2022201999, "Ivan"},
        {2024103666, "Judy"},
    };

    int n = sizeof(dataset) / sizeof(dataset[0]);
    for (int i = 0; i < n; i++) {
        s.id = dataset[i].id;
        strncpy(s.name, dataset[i].name, sizeof(s.name));
        s.name[sizeof(s.name)-1] = '\0';
        if (ioctl(fd, STUDENT_ADD, &s) < 0) {
            perror("ioctl ADD");
        } else {
            printf("Added: %d %s\n", s.id, s.name);
        }
    }

    /* 按名字查询 */
    strcpy(s.name, "Alice");
    if (ioctl(fd, STUDENT_QUERY_NAME, &s) == 0) {
        printf("Query by name=Alice -> id=%d\n", s.id);
    } else {
        perror("ioctl QUERY_NAME");
    }

    strcpy(s.name, "Judy");
    if (ioctl(fd, STUDENT_QUERY_NAME, &s) == 0) {
        printf("Query by name=Judy -> id=%d\n", s.id);
    } else {
        perror("ioctl QUERY_NAME");
    }

    /* 按年级查询 */
    int grade = 2023;
    printf("\nQuery grade %d (see kernel log for results)\n", grade);
    ioctl(fd, STUDENT_QUERY_GRADE, &grade);

    grade = 2022;
    printf("\nQuery grade %d (see kernel log for results)\n", grade);
    ioctl(fd, STUDENT_QUERY_GRADE, &grade);

    /* 按学院查询 */
    int college = 103;
    printf("\nQuery college %d (see kernel log for results)\n", college);
    ioctl(fd, STUDENT_QUERY_COLLEGE, &college);

    college = 201;
    printf("\nQuery college %d (see kernel log for results)\n", college);
    ioctl(fd, STUDENT_QUERY_COLLEGE, &college);

    close(fd);
    return 0;
}
