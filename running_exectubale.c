#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>

int main()
{
	pid_t pid;
	char *args[2];
	char size = 100;
	char buffer[size];

	while(1)
	{
		char string[100];

		if(getcwd(buffer, size) != NULL) 
			printf("Enter the executable you want to run\npavan:<%s>$", buffer);

		fgets(string , sizeof(string), stdin);

		string[strcspn(string, "\n")] = '\0';

		if(string[0] != 'c' || string[1] != 'd' ) {
			pid = fork();
			if(pid == 0) {
				args[0] = string;
				args[1] = NULL;

				execv(args[0] , args);
			}
			else if(pid > 0)
				wait(NULL);
		}
		else{
			char *temp_string;

			if ((temp_string = strchr(string, '/')) != NULL)
			{
				/* temp_string now points to '/' */
			}
			else
				if (strncmp(string, "cd ", 3) == 0)
					temp_string = string  + 3;

			struct stat sb;

			if (stat(temp_string, &sb) !=  0)
				printf("Path is invalid\n");

			chdir(temp_string);
		}
	}
}
