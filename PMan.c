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
    char name[100];
    struct Process* next;
} Process;

typedef struct {
    Process* head;
} LinkedList;

/* Adds the specified process to the LinkedList */
void addProcessFront(LinkedList* L, pid_t pid, char* namePtr){
    Process* toAdd = malloc(sizeof(Process));
    toAdd->pid = pid;
    strcpy(toAdd->name, strcat(namePtr, "\0"));
    toAdd->next = L->head;
    L->head = toAdd;
}

/* Removes a process from the activeProcesses list */
void removeProcess(Process* prev, Process* toRemove, LinkedList* L){
    if(toRemove == L->head){
        //remove first element
        L->head = toRemove->next;
    }
    else if(toRemove->next == NULL){
        //remove last element
        prev->next = NULL;
    }
    else{
        prev->next = toRemove->next;
    }
    free(toRemove);
}

/* Prints the contents of activeProcesses list */
void printActiveProcesses(LinkedList* L){
    Process* current = L->head;
    if (current == NULL){
        printf("No background processes.\n");
        return;
    }
    int count = 0;
    while(current != NULL){
        printf("%d: %s\n", current->pid, current->name);
        current = current->next;
        count++;
    }
    printf("Total background jobs: %d\n", count);
}

/* Updates activeProcesses list using waitpid() */
void updateActiveProcesses(LinkedList* L){
    Process* current = L->head;
    Process* prev = NULL;

    while(current != NULL){
        //get the status of the current process
        printf("At process %s\n", current->name);
        int status;
        pid_t result = waitpid(current->pid, &status, WNOHANG);
        
        if(result == -1){
            printf("Error in waitpid() call\n");
        }
        else if(result != 0){
            //printf("Remove process %s\n", current->name);
            removeProcess(prev, current, L);
        }
        prev = current;
        current = current->next;
    }
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

void executeCmd(int length, char** parsedCmd, LinkedList* activeProcesses){
    if(strcmp((*parsedCmd), "bg") == 0){
        pid_t pid = fork();
        if(pid == 0){
            char* args[] = { parsedCmd[1], NULL };
            execvp(args[0], args);
        }
        else if(pid > 0){
            addProcessFront(activeProcesses, pid, parsedCmd[1]);
        }
    }
    else if(strcmp((*parsedCmd), "bglist") == 0){
        printActiveProcesses(activeProcesses);
    }
    else if(strcmp((*parsedCmd), "bgkill") == 0){
        pid_t pid = (pid_t)atoi(parsedCmd[2]);
        kill(pid, SIGTERM);
    }else{
        printf("Error: (%s) unrecognized command\n", *parsedCmd);
    }
}

int main(){
    LinkedList activeProcesses;
    activeProcesses.head = NULL;

    int run = 1;
    char* input;
    char* parsedCmd[MAXCMD];

    while(run){
        printActiveProcesses(&activeProcesses);

        input = readline("PMan: > ");

        if (strlen(input) == 0)
            continue;
        add_history(input);

        int inputlength = parseInput(strcat(input, "\0"), &parsedCmd[0]);

        if(strcmp(input, "exit") == 0){
            exit(0);
        }
        
        updateActiveProcesses(&activeProcesses);

        executeCmd(inputlength, &parsedCmd[0], &activeProcesses);

        freeParsed(inputlength, parsedCmd);
        free(input);
        
    }
    return 0;
}