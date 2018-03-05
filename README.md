**REQ 1: Present the user with a prompt at which he or she can enter commands.**

The user is presented with a promp of "[421shell] >" until they choose to exit the program with CTRL-C as was described to us. Their commands are processed through the readUserInput() function. 

**REQ 2: Accept command input of arbitrary length.**

Within readUserInput() there is a default buffer size for how long the user's input can be for commands. This of course has to be able to be arbitrarily long, so the buffer holding the user's input commands is dynamically allocated. If their command(s) exceed the length of the buffer, the buffer size is increased dynamically again. 

**REQ 3: Parse command-line arguments from the user's input and pass them along to the program that the user requests to be started.**

Once we have read the user's input in readUserInput(), we process their actual arguments into 'tokens' within the readArgs() function. The 'tokens' are just pointers to pointers (so, just a list). Once we have our arguments the user sent parsed into tokens then we just have to execute them within our exeArgs() function. Here is where we actually pass the the user's command-line arguments to the program that they requested to be started. 

There is of course intricate details to the program and how it works to achieve these requirements, and this is just a brief overview of how the shell does what it is assigned to do. There are more extensive comments within the simple_shell.c file explaining my thoughts and implementation.
