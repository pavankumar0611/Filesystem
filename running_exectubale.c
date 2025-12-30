#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
        pid_t pid;
        char *args[2];

        while(1)
        {

                pid = fork();
                if(pid == 0) {
                        char string[10];
                        printf("Enter the executable you want to run\npavan>");
                        fgets(string , sizeof(string), stdin);

                        string[strcspn(string, "\n")] = '\0';

                        args[0] = string;
                        args[1] = NULL;

                        execv(args[0] , args);
                }
                else if (pid > 0) {
                        wait(NULL);
        }
                else
                        printf("still running\n");
        }

}
