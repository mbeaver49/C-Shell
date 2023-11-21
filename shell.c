/*
 * This program runs a simple shell that should do the following:
 * display prompt, get input from the user, execute commands (exit, cd),
 * execute programs, redirect input/output, and repeat.
 *
 * author: Manvel Beaver and Dr. Heidi Gurung
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#define MAX 512

//Prototypes functions
void parseArgs(char* buffer, char** args, int argSize, int *nargs);
void checkRedirectOutput(int* nargs, char** args);
void checkRedirectInput(int* nargs, char** args);

int main() {
  // Environment setup
  int numArgs = 0;
  char* buffer = (char*) malloc(sizeof(char) * MAX);
  char** args = (char**) malloc(sizeof(char*) * MAX);
  int argsSize = MAX;
  int *nargs = &numArgs;

  while(1){
    // Print prompt
    char* cwd = getcwd(NULL, 0);//This only works in a linux environment
    printf("\n%s>", cwd);

    // Accept input from user
    fgets(buffer, MAX, stdin);

    // Parse input
    parseArgs(buffer, args, argsSize, nargs);

    // Verify input has been entered (the user did not just hit 'Enter')
    if (*nargs > 0) {
    // Handle some internal commands (e.g. exit and cd)
      if (!strcmp(args[0], "exit")) {
        printf("Now Exiting\n");
        exit(0);	
      }
      if (!strcmp(args[0], "cd")){
        cwd = strcat(cwd, "/");
        cwd = strcat(cwd, args[1]);
        chdir(cwd);
      } else { // Not an internal command so we need to fork off a process
        pid_t pid;
        int status;
        // Fork
        pid = fork();
        // Run the external process
        if(pid < 0){
          printf("Fork Failed\n");
        }
        if(pid == 0){//child
          checkRedirectOutput(nargs, args);
          checkRedirectInput(nargs, args);
          execvp(args[0], args);
          printf("Execvp failed\n");

        }else{//parent
          wait(&status);
        }		
        // Remove this break; when you are confident the rest is working.
        // Otherwise you risk crashing not only the program but a whole server.
        //break;  
      }
    }else{
      printf("\n (-_-)   Enter something mortal!!!");//funny output
      printf("\n  |_|");
      printf("\n   LL ");
    }
  }
  //Free which has been malloced
  free(args);
  free(buffer);
  return 0;
}


// buffer is the input string of characters
// args is the output array of arguments.  It has already been created with argsSize slots.
// nargs as the number filled in that is passed back
void parseArgs(char *buffer, char** args, int argsSize, int *nargs) {
  char *bufArgs[argsSize];
  char **cp;
  char *wbuf;
  int i, j;
  
  wbuf=buffer;
  bufArgs[0]=buffer; 
  args[0]=buffer;

  for(cp=bufArgs; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
    if ((*cp != '\0') && (++cp >= &bufArgs[argsSize]))
      break;
  }
    
  for (j=i=0; bufArgs[i]!=NULL; i++){
    if(strlen(bufArgs[i])>0)
      args[j++]=bufArgs[i];
  }
    
  // Add the NULL as the end argument because we need that for later
  *nargs=j;
  args[j]=NULL;

}


/*These are void functions that check for the redirect symbols
 * , changes the args string array to fit the freopen method that
 * either redirect input or output based on parameters.
 */
void checkRedirectOutput(int* nargs, char** args){
	int i;
	for(i = 0; i < (*nargs-1); i++){
		if(!strcmp(args[i], ">")){
			printf("\nRedirecting output...\n");
			args[i] = args[i+1];
			args[i+1] = "\0";	
			freopen(args[i], "w", stdout);
		}
	}
}
void checkRedirectInput(int* nargs, char** args){
	int i;
	for(i = 0; i < *nargs; i++){
		if(!strcmp(args[i], "<")){
			printf("\nRedirecting input...\n");
			args[i] = args[i+1];
			args[i+1] = "\0";
			freopen(args[i], "r", stdin);
		}
	}
}


