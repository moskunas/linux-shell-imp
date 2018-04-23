/*
CMSC 421 UMBC 2018
Ian Moskunas / ianmosk1@umbc.edu
Homework 2 Part II
Started 4.21.2018

Second iteration of my shell program. Adding more functionality on top of what was done in HW1.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

#include "utils.h"

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
	// Using to implement required built-in's (there are 4)
	char *reqBuiltIn[4] ; 
	char *spaceFiller = " " ;
	char echoOutput[PATH_MAX] ;		
	int argPlace ;
	FILE *filePtr ;
	
	// chdir and cd will do same thing just have diff names
	reqBuiltIn[0] = "exit" ;
	reqBuiltIn[1] = "chdir" ;
	reqBuiltIn[2] = "cd" ;
	reqBuiltIn[3] = "echo" ;
	
	//User hit enter and nothing else
	if (argTokens[0] == NULL)
		return 0 ;
	
	// See if we need to deal with a built-in arg requirement
	int j ;
	for (j = 0 ; j < 4 ; j++)
	{
		if (strcmp(argTokens[0] , reqBuiltIn[j]) == 0)
		{
			argPlace = j + 1 ;
			break ;
		}
	}
	
	switch (argPlace)
	{
	
	// "exit" built-in
	case 1:
	
		// Too many arguments given
		if (argTokens[2] != NULL)
		{
			printf("%s\n" , "ERROR: Can't execute this command") ;	
			return 0 ;
		}
	
		// No argument given
		if (argTokens[1] == NULL)
			exit(0) ;
		
		// Argument given but isn't int
		for (j = 0 ; j < strlen(argTokens[1]) ; j++)
		{
			if (argTokens[1][j] > '9' || argTokens[1][j] < '0')
			{
				printf("%s\n" , "ERROR: Can't execute this command") ;	
				return 0 ;
			}
		}		
	
		// Only one arg, is int, so can exit with it
		exit(atoi(argTokens[1])) ;
		return 0 ;

	// "chdir" built-in
	case 2:
	
		// No argument given case, go home if possible
		if (argTokens[1] == NULL)
		{
			if (chdir(getenv("HOME")) < 0)
			{
				printf("%s\n" , "ERROR: Can't execute this command") ;
				return 0 ;
			}
			
			setenv("PWD" , "home" , 1) ;
			return 0 ;
		}
		
		// Unescaping prior to use
		filePtr = fopen("/usr/src/homework2/possErr.txt" , "w") ;
		strcat(echoOutput , unescape(argTokens[1] , filePtr)) ;
		argTokens[1] = echoOutput ;
		fclose(filePtr) ;
				
		// Only one arg, can move forward normally 
		chdir(argTokens[1]) ;
		setenv("PWD" , argTokens[1] , 1) ;
		
		// Clearing echo's output buffer
		j = 0 ;
		while (echoOutput[j] != 0)
		{
			echoOutput[j] = 0 ;
			j++ ;
		}
			
		return 0 ;
		
	// "cd" built-in (same as case 1 per requirements)
	case 3:

		// No argument given case, go home if possible
		if (argTokens[1] == NULL)
		{
			if (chdir(getenv("HOME")) < 0)
			{
				printf("%s\n" , "ERROR: Can't execute this command") ;
				return 0 ;
			}
			
			setenv("PWD" , "home" , 1) ;
			return 0 ;
		}
		
		// Unescaping prior to use
		filePtr = fopen("/usr/src/homework2/possErr.txt" , "w") ;
		strcat(echoOutput , unescape(argTokens[1] , filePtr)) ;
		argTokens[1] = echoOutput ;
		fclose(filePtr) ;
		
		// Only one arg, can move forward normally 
		chdir(argTokens[1]) ;
		setenv("PWD" , argTokens[1] , 1) ;
		
		
		// Clearing echo's output buffer
		j = 0 ;
		while (echoOutput[j] != 0)
		{
			echoOutput[j] = 0 ;
			j++ ;
		}
		
		return 0 ;

	// "echo" built-in
	case 4:
	
		// Unescaping prior to use
		filePtr = fopen("/usr/src/homework2/possErr.txt" , "w") ;
				
		j = 1;
		while (argTokens[j] != NULL)
		{
			// Have to account for spaces that are skipped previously
			if (j > 1)
				strcat(echoOutput , spaceFiller) ;
			
			strcat(echoOutput , unescape(argTokens[j] , filePtr)) ;
			j++ ;
		}
		
		fclose(filePtr) ;
		printf("%s\n" , echoOutput) ;		
		
		// Clearing echo's output buffer
		j = 0 ;
		while (echoOutput[j] != 0)
		{
			echoOutput[j] = 0 ;
			j++ ;
		}

		return 0 ;

	default:
		break ;
	}


	//Unescaping things that aren't bult-in per the requirements
	if (argTokens[1] != NULL)
	{
		filePtr = fopen("/usr/src/homework2/possErr.txt" , "w") ;
		strcat(echoOutput , unescape(argTokens[1] , filePtr)) ;
		
		argTokens[1] = echoOutput ;
		
		fclose(filePtr) ;		
	}

	pid_t pid, waitpid ;
	pid = fork() ; 
	
	// Successful fork(), continue to exec()
	if(pid == 0)
	{
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
		wait(NULL) ;
	}
	
	// Clearing echo's output buffer
	j = 0 ;
	while (echoOutput[j] != 0)
	{
		echoOutput[j] = 0 ;
		j++ ;
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
	
	while(1)
	{	
		fprintf(stdout , "[421shell] > ") ;
		userInput = readUserInput() ;
		argTokens = readArgs(userInput) ;
		userCommands = exeArgs(argTokens) ; 
	}
	return 0  ;
}
