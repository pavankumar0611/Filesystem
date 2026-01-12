#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

#define SIZE 100

void trim_trailing_space(char *s) {
	int  len = strlen(s);
	/* for removing railing spaces */
	while (len > 0 && isspace((unsigned char)s[len - 1])) {
		s[--len] = '\0';
	}   
}

char *parse_commandline_argument(char *string)
{
	/* skip leading whitespace */
	while (isspace((unsigned char)*string))
		string++;

	/* must start with "cd" */
	if (strncmp(string, "cd", 2) != 0)
		return NULL;

	string += 2;

	/* skip whitespace after cd */
	while (isspace((unsigned char)*string))
		string++;

	return string;
}



char *different_path_string(const char **string, char *new_appended_path, char *commandline_argument_passed)
{
	int  i = 0;
	while(**string  != '\0' && **string != ':') {
		new_appended_path[i++] = *(*string)++;
	}
	new_appended_path[i++] = '/';

	while( *commandline_argument_passed != '\0') {
		new_appended_path[i++] = *commandline_argument_passed;
		commandline_argument_passed++;
	}
	new_appended_path[i] = '\0';

	if(**string == ':')
		(*string)++;

	return new_appended_path;
}

int main()
{
	pid_t pid;
	char *args[2];
	char buffer[SIZE];
	int status;

	while(1)
	{
		char string[SIZE];

		if(getcwd(buffer, SIZE) != NULL)
			printf("Enter the executable you want to run\npavan:<%s>$", buffer);

		fgets(string , sizeof(string), stdin);

		string[strcspn(string, "\n")] = '\0';
		if(string[0] == 'c' && string[1] == 'd' ) {
			char *temp_string = parse_commandline_argument(string);

			struct stat sb;
			trim_trailing_space(temp_string);
			if (stat(temp_string, &sb) !=  0)
				printf("Path is invalid\n");

			chdir(temp_string);

		}
		else if (access(string , X_OK) != 0) {

			const char *environment_path = getenv("PATH");
			char *args[2];
			pid_t pid;
			char fullpath_of_directory[SIZE];
			int status;

			for(;*environment_path  != '\0';) {

				different_path_string(&environment_path, fullpath_of_directory, string);

				if(fullpath_of_directory == NULL){
					printf("environment variable is NULL\n");
					exit(1);
				}

				if(access(fullpath_of_directory, X_OK) == 0) {
					status = 1;
					pid = fork();
					if(pid == 0) {
						args[0] = fullpath_of_directory;
						args[1] = NULL;

						execv(args[0] , args);
					}
					else if(pid > 0)
						wait(NULL);

					printf("*************ls found at %s****************\n", fullpath_of_directory);
					break;
				}

			}
			if(!status)
				printf("the %s not found\n", string);

		}
		else{
			pid = fork();
			if(pid == 0) {
				args[0] = string;
				args[1] = NULL;

				execv(args[0] , args);
			}
			else if(pid > 0)
				wait(NULL);

		}
	}
}
