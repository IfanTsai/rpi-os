#include "user.h"
#include "unistd.h"

void loop(char* str)
{
    char buf[2] = { 0 };

    for (;;) {
        for (int i = 0; i < 5; i++) {
            buf[0] = str[i];
            write(buf);
            user_delay(100000);
        }
    }
}

void user_process() 
{
    write("user process started\r\n");
    int pid = fork();
    if (pid < 0) {
        write("failed to fork\r\n");
        exit();
    }

    if (pid == 0)
        loop("abcde");
    else
        loop("12345");
}