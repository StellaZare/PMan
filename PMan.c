#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCMD 100 //Max words in input command

/* Prints elements of char* array given the length - for testing */
void printList(int length, char* list[length]){
    for (int i = 0; i < length; i++)
    {
        printf("%d. %s\n", i, list[i]);
    }  
}

/* Frees the memory used by parsedCmd array */
void freeParsed(int length, char* list[length]){
    for (int i = 0; i < length; i++)
    {
        free(list[i]);
    }  
}

/* Prints current working directory to stdout - for testing */
void printDirectory() {
    char directory [1000];
    if ( getcwd(directory, sizeof(directory)) != NULL){
        printf("Current Directory: %s\n", directory);
    }else{
        printf("Error in getcwd() call\n");
    }
   
}

/* Parse input: creates a array for the input */
int parseInput(char* input, char** parsedCmd) {
    int parsedIdx = 0;

    char* token = strtok(input, " ");
    while (token != NULL) {
        parsedCmd[parsedIdx] = malloc(strlen(token) + 1);
        strcpy(parsedCmd[parsedIdx++], token);
        token = strtok(NULL, " ");
    }

    return parsedIdx;
}

void executeCmd(int length, char** parsedCmd){
    if(strcmp((*parsedCmd), "bg") == 0){
        char* args[] = { parsedCmd[1], NULL };
        execvp(args[0], args);
    }
    else if(strcmp((*parsedCmd), "bglist") == 0){

    }
    else if(strcmp((*parsedCmd), "exit") == 0){
        printf("Exit()\n");
    }else{
        printf("Error: (%s) unrecognized command\n", *parsedCmd);
    }
}

int main(){
    printDirectory();
    int run = 1;
    char* input;
    char* parsedCmd[MAXCMD];

    while(run){
        input = readline("PMan: > ");
        if (strlen(input) == 0)
            continue;
        add_history(input);

        int inputlength = parseInput(strcat(input, "\0"), &parsedCmd[0]);
        //printList(inputlength, parsedCmd);

        pid_t pid = fork();
        //execution of child starts here

        if(pid < 0){
            printf("Error in fork() call\n");
        }
        else if (pid == 0){
            //inside child process
            executeCmd(inputlength, &parsedCmd[0]);
        }
        else{
            //inside parent process
            wait(NULL);
            free(input);
            freeParsed(inputlength, parsedCmd);
        }
        
    }
    return 0;
}