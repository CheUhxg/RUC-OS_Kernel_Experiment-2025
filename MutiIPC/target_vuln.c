/*
 Simple target program:
 - 从 stdin 读取一行
 - 如果行中包含 "CRASH" 则触发 abort()（导致 SIGABRT）
 - 否则 正常 exit(0)
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buf[512];
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    if (strstr(buf, "CRASH")) {
        fprintf(stderr, "target_vuln: triggered crash for input: %s", buf);
        abort(); /* 模拟崩溃 */
    }
    printf("target_vuln: ok input: %s", buf);
    return 0;
}
