/*
CMSC 421 UMBC 2018
Ian Moskunas / ianmosk1@umbc.edu
Homework 1 Part II
Started 3.3.2018

First iteration of my shell program, will only need to support a few very basic features of a full fledged *nix shell. **Will be doing extra comments because trying to understand how I get this to work**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFF_SIZE 500


// Reads line of input from user and accomodates arbitrary length 
char *readUserInput()
{
	// Will be adjustable arbitrary size for user entries 
	int bufferSize = BUFF_SIZE ;
	// Will actually be holding user entries into shell for interpretation 
	char *buffer = malloc(sizeof(char) * bufferSize) ; 
	// Holds current char in user input 
	int currentChar ;
	
	// Get characters until reach end of the current user input 
	int currPos = 0 ;
	while(1)
	{
		currentChar = fgetc(stdin) ; 
		if(currentChar == '\n' || currentChar == EOF)
		{
			buffer[currPos] = '\0' ; 
			return buffer ;
		}
		
		else
		{
			buffer[currPos] = currentChar ; 
		}
		
		currPos++ ;
		
		// Have to accomodate arbitrary user input length: 
		if(currPos >= bufferSize)
		{
			bufferSize += BUFF_SIZE ;
			// Resize buffer to hold user input 
			buffer = realloc(buffer , bufferSize) ;
		}
	}
}


// We are turning each user input into seperate arguments that we can execute for the user 
// Basically same things we did with readUserInput() but now with the args within the entries 
char **readArgs(char *userInput)
{
	int bufferSize = BUFF_SIZE , userPos = 0 ;
	// We will hold all of the user args in an array of 'tokens' via pointers: 
	char **argTokens = malloc(sizeof(char*) * bufferSize) ;
	char *singleToken ;
	char *restUserInput = userInput ;
	
	//Splitting user input into tokens with strtok_r() that was recommended in assignment: 
	while(singleToken = strtok_r(restUserInput , " " , &restUserInput))
	{
		//printf("TESTING TOKEN: %s\n" , singleToken) ;
		
		argTokens[userPos] = singleToken ;
		userPos++ ;
		
		// Accomodate arbitrary # of tokens 
		if(userPos >= bufferSize)
		{
			bufferSize += BUFF_SIZE ;
			argTokens = realloc(argTokens , sizeof(char*) * bufferSize) ;
		}
	}
	argTokens[userPos] = NULL ;
	return argTokens ;
}

// Executing the arguments that are in the array of pointers that user entered
int exeArgs(char **argTokens)
{
	pid_t pid, waitpid ;
	// Fork process and save return value, doing different things depending on fork() result 
	pid = fork() ; 
	
	// Successful fork(), continue to exec()
	if(pid == 0)
	{
		// Child section here:
	
		//printf("%s\n" , "fork() successful...") ;
		execvp(argTokens[0] , argTokens) ;
		
		// Bad shell command so child never executes , give error and kill useless child:
		printf("%s\n" , "ERROR: Can't execute this command") ;
		exit(1) ;
	}
	
	// fork() failed
	else if(pid < 0)
	{
		printf("%s\n" , "ERROR: fork() failed...") ;
	}	
	else
	{
		// This is for the parent process since the child went and did their own thing
		wait(NULL) ;
	}
	
	// Because we want to keep prompting the user for input like a normal shell
	return 0;
}


int main()
{		
	// Holds the entire user input for the given line */
	char *userInput ; 
	// Holds 'tokens' of user input if >1 args are delimited by spaces as said in proj. desc.
	char **argTokens ; 
	int userCommands ;
	
	// Prompt user for commands until they CTRL-C 
	while(1)
	{	
		fprintf(stdout , "[421shell] > ") ;
		userInput = readUserInput() ;
		argTokens = readArgs(userInput) ;
		userCommands = exeArgs(argTokens) ; 
	}
	return 0  ;
}
