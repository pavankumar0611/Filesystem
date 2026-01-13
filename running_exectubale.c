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

//Check if the user passed the redirection to a file and save the file name if passed.

int to_check_if_the_string_contains_redirection_to_file (char *original_string , int tocheck_redirection ,char *outfile_name) {

	char *temp_string = original_string;

	int i = 0;
	while(temp_string[i++] != '\0') {

		if(temp_string[i] == '>') {
			tocheck_redirection = 1;
			break;
		}
	}

	if(tocheck_redirection) {
		strcpy(outfile_name, &temp_string[i+ 2]);

		original_string[i] = '\0';
		return 1;
	}
	else
		return 0;
}

//For the given shell exectuable commands check in path environment variable
//appended to absolute path save in new_appended_path

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
	char *args[5], output_file_name[SIZE/2];
	char buffer[SIZE];
	int status, generating_output_to_file ;
	// ANSI escape code for green text
	const char* green = "\033[0;32m";
	// ANSI escape code to reset text color to default
	const char* reset = "\033[0m";


	while(1)
	{
		char string[SIZE];
		generating_output_to_file = 0;

		if(getcwd(buffer, SIZE) != NULL)
			printf("%spavan:<%s>$%s",green ,buffer, reset);

		fgets(string , sizeof(string), stdin);
		string[strcspn(string, "\n")] = '\0';

		generating_output_to_file = to_check_if_the_string_contains_redirection_to_file(string , generating_output_to_file ,output_file_name);
		trim_trailing_space(string);

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
			char fullpath_of_directory[SIZE], *temporary_full_path;
			int status;

			for(;*environment_path  != '\0';) {

				different_path_string(&environment_path, fullpath_of_directory, string);

				if(fullpath_of_directory == NULL){
					printf("environment variable is NULL\n");
					exit(1);
				}

				int i = 0;
				temporary_full_path = fullpath_of_directory;

				char *token = strtok(fullpath_of_directory, " ");
				while (token && i < 5) {
					args[i++] = token;
					token = strtok(NULL, " ");
				}
				args[i] = NULL;
				temporary_full_path[strcspn(temporary_full_path, " ")] = '\0';

				if(access(temporary_full_path, X_OK) == 0) {
					status = 1;
					pid = fork();
					if(pid == 0) {
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

				if(generating_output_to_file) {
					FILE *fp =      freopen( output_file_name, "w" , stdout);
					if(fp == NULL)
						printf("Error generating the output into a file\n");
				}

				execv(args[0] , args);
			}
			else if(pid > 0)
				wait(NULL);

		}
	}
}
