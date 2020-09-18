/*
 2020-2 [OPERATING SYSTEMS] Assignment #1
 JuYoung Oh / 20161872 / Electrical & Electronics Engineering
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

/*Define prompt text color*/
#define RED "\x1B[1;91m" 
#define BLUE "\x1B[1;94m"
#define GREEN "\x1B[1;92m"
#define RESET "\x1B[0m"

/*Define maximum length of arrays*/
#define MAX_COMMAND_LENGTH 128
#define MAX_FILE_NAME_LENGTH 16
#define MAX_PARAM_LENGTH 16

/*Declaration of functions*/
void type_prompt();
void read_command(char* command, char* file_name, char** parameters);

int main(){
	char command[MAX_COMMAND_LENGTH];
	char file_name[MAX_PARAM_LENGTH];
	char* parameters[MAX_PARAM_LENGTH];
	pid_t pid;
	int error_code;
	
	while(1){
		type_prompt();
		fgets(command, sizeof(command), stdin);

		if(strcmp(command, "\n") == 0)
			continue;
		
		command[strlen(command) - 1] = '\0';

		if(strcmp(command, "exit") == 0)
			break;
		
		if(!(command[0] == '\33' && command[2] == 'A')){
			parameters[0] = NULL;
			read_command(command, file_name, parameters);
		}
				
		pid = fork();

		switch(pid){
			case -1: //Fork Failure
				printf("Fork Error\n");
				break;
			case 0: //Child
				if (execvp(file_name, parameters) == -1){
					printf(RED "Command Execution Error\n" RESET);
					exit(EXIT_SUCCESS);
				}
			default: //Parent
				wait(NULL);
		}
	}
}

/*Definition of functions*/
void type_prompt(){
	printf(GREEN "JuYoung Oh:");
	printf(BLUE "~");
	printf(RESET "$ ");
}

void read_command(char* command, char* file_name, char** parameters){
	int i = 0;
	char* token = strtok(command, " ");

	while(token != NULL){
		parameters[i++] = token;
		token = strtok(NULL, " ");
	}
	
	parameters[i] = NULL;
	
	if(parameters[0] != NULL)
		strcpy(file_name, parameters[0]);	
}
