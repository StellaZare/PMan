#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXCMD 100 //Max words in input command

typedef struct Process{
    pid_t pid;
    char* identifer;
    struct Process* next;
} Process;

typedef struct {
    Process* head;
} LinkedList;

/* Adds the specified process to the LinkedList */
void addProcessFront(LinkedList* L, pid_t pid, char* identifier){
    Process* toAdd = malloc(sizeof(Process));
    toAdd->pid = pid;
    toAdd->identifer = identifier;
    toAdd->next = L->head;
    L->head = toAdd;
}

/* Prints the contents of LinkedList. If empty prints empty message. */
void printProcesses(LinkedList* L){
    Process* current = L->head;
    if (current == NULL){
        printf("No background processes\n");
        return;
    }
    int count = 0;
    while(current != NULL){
        printf("%d: %s\n", current->pid, current->identifer);
        current = current->next;
        count++;
    }
    printf("Total background jobs: %d\n", count);
}

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

void executeCmd(int length, char** parsedCmd, LinkedList* processes){
    if(strcmp((*parsedCmd), "bg") == 0){
        addProcessFront(processes, getpid(), parsedCmd[1]);
        pid_t pid = fork();
        if(pid == 0){
            char* args[] = { parsedCmd[1], NULL };
            execvp(args[0], args);
            exit(0);
        }
    }
    else if(strcmp((*parsedCmd), "bglist") == 0){
        printProcesses(processes);
    }
    else if(strcmp((*parsedCmd), "bgkill") == 0){
        pid_t pid = (pid_t)atoi(parsedCmd[2]);
        kill(pid, SIGTERM);
    }else{
        printf("Error: (%s) unrecognized command\n", *parsedCmd);
    }
    exit(0);
}

int main(){
    LinkedList processes;
    processes.head = NULL;

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
            executeCmd(inputlength, &parsedCmd[0], &processes);
            exit(0);
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