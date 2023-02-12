#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
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
        printf("No background processes\n");
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
        int status;
        pid_t result = waitpid(current->pid, &status, WNOHANG);
        
        if (result == -1){
            printf("Error in waitpid()\n");
        }
        if(result != 0){
            printf("[%d] %s has terminated\n", current->pid, current->name);
            removeProcess(prev, current, L);
        }
        prev = current;
        current = current->next;
    }
}

/* Prints elements of char* array given the length - for testing */
void printList(int length, char* list[length]){
    for (int i = 0; i < length; i++)
        printf("%d. %s\n", i, list[i]);
}

/* Frees the memory used by parsedCmd array */
void freeParsed(int length, char* list[length]){
    for (int i = 0; i < length; i++)
        free(list[i]); 
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

/* creates the  args array used in call to exec */
void createArgs(int length, char** parsedCmd, char* args[]){
    for(int i = 1; i < length; i++){
        args[i-1] = parsedCmd[i];
    }
    args[length-1] = NULL;
}

/* checks the input command legth used for error handling `*/
int checkCommand(int length){
    if(length < 2){
        printf("Error: Incomplete command\n");
        return -1;
    }
    return 0;
}

void executeStat(pid_t pid){
    int arraySize = 100;
    char buffer[arraySize];

    // comms
    char comm[arraySize];
    snprintf(comm, arraySize, "/proc/%d/comm", pid);
    FILE* commFile = fopen(comm, "r");
    if(commFile == NULL){
        printf("Error: fopen() call failed");
        return;
    }
    fgets(comm, arraySize, commFile);
    fclose(commFile);

    // state
    char state;
    char rss[arraySize];
    char voluntary[arraySize];
    char nonvoluntary[arraySize];
    char status[arraySize];
    snprintf(status, arraySize, "/proc/%d/status", pid);
    FILE* statusFile = fopen(status, "r");
    if (statusFile == NULL){
        printf("Error: fopen() call failed");
        return;
    }
    while(fgets(buffer, arraySize, statusFile)){
        if(strncmp(buffer, "State:", 6) == 0){
            state = buffer[7];
        }
        if (strncmp(buffer, "VmRSS:", 6) == 0) {
            char* token = strtok(buffer, "\t");
            token = strtok(NULL, " ");
            strcpy(rss, token);
        }
        if (strncmp(buffer, "voluntary_ctxt_switches:", 24) == 0) {
            char* token = strtok(buffer, "\t");
            token = strtok(NULL, " ");
            strcpy(voluntary, token);
        }
        if (strncmp(buffer, "nonvoluntary_ctxt_switches:", 27) == 0) {
            char* token = strtok(buffer, "\t");
            token = strtok(NULL, " ");
            strcpy(nonvoluntary, token);
        }
    }
    fclose(statusFile);

    // utime 
    char utime[arraySize], stime[arraySize];
    char stat[arraySize];
    snprintf(stat, arraySize, "/proc/%d/stat", pid);
    FILE* statFile = fopen(stat, "r");
    if(statFile == NULL){
        printf("Error: fopen() call failed");
        return;
    }
    fgets(buffer, arraySize, statFile);
    char* token = strtok(buffer, " ");
    int count = 1;
    while(token && count < 16){
        if(count == 14){
            strcpy(utime, token);
        }
        if(count == 15){
            strcpy(stime, token);
        }
        token = strtok(NULL, " ");
        count++;
    }
    fclose(statFile);

    // printf satements
    printf("-----\n");
    printf("PID\t %d\n", pid);
    printf("comm\t %s", comm);
    printf("state\t %c\n", state);
    printf("utime\t %s\n", utime);
    printf("stime\t %s\n", stime);
    printf("rss\t %s\n", rss);
    printf("voluntary switch\t %d\n", atoi(voluntary));
    printf("nonvoluntary switch\t %d\n", atoi(nonvoluntary));
    printf("-----\n");

}

void executeCmd(int length, char** parsedCmd, LinkedList* activeProcesses){
    int killResult = 0;

    if(strcmp((*parsedCmd), "bg") == 0){
        if (checkCommand(length) == -1){
            return;
        }
        pid_t pid = fork();
        if(pid == 0){
            // to terminate child if parent was terminated 
            prctl(PR_SET_PDEATHSIG, SIGTERM);
            // parse input array
            char* args[length];
            createArgs(length, parsedCmd, &args[0]);
            
            int execResult = execvp(args[0], args);
            if (execResult == -1){
                printf("Error: (%s) not found\n", parsedCmd[1]);
                exit(1);
            }
        }
        else if(pid > 0){
            usleep(200000);
            int status;
            pid_t execResult = waitpid(pid, &status, WNOHANG);
            if (execResult == 0){
                addProcessFront(activeProcesses, pid, parsedCmd[1]);
            }
        }
    }
    else if(strcmp((*parsedCmd), "bglist") == 0){
        printActiveProcesses(activeProcesses);
    }
    else if(strcmp((*parsedCmd), "bgkill") == 0){ 
        if (checkCommand(length) == -1){
            return;
        } 
        pid_t pid = (pid_t)atoi(parsedCmd[1]);
        killResult = kill(pid, SIGTERM);
        if(killResult == 0)
            printf("Process [%d] killed\n", pid);
          
    }
    else if(strcmp((*parsedCmd), "bgstop") == 0){
        if (checkCommand(length) == -1){
            return;
        }
        pid_t pid = (pid_t)atoi(parsedCmd[1]);
        killResult = kill(pid, SIGSTOP);
        if(killResult == 0)
            printf("Process [%d] stopped\n", pid);
    }
    else if(strcmp((*parsedCmd), "bgstart") == 0){
        if (checkCommand(length) == -1){
            return;
        }
        pid_t pid = (pid_t)atoi(parsedCmd[1]);
        killResult = kill(pid, SIGCONT);
        if(killResult == 0)
            printf("Process [%d] started\n", pid);

    }
    else if(strcmp((*parsedCmd), "pstat") == 0){
        if (checkCommand(length) == -1){
            return;
        }
        pid_t pid = (pid_t)atoi(parsedCmd[1]);
        executeStat(pid);
    }else{
        printf("Error: Unrecognized command %s\n", *parsedCmd);
    }  
    if(killResult == -1)
        printf("Error: Process [%s] does not exist\n", parsedCmd[1]);

}

int main(){
    LinkedList activeProcesses;
    activeProcesses.head = NULL;
    char* input;
    char* parsedCmd[MAXCMD];

    while(1){
        input = readline("PMan: > ");
        updateActiveProcesses(&activeProcesses);

        if (strlen(input) == 0)
            continue;
        add_history(input);

        int inputlength = parseInput(strcat(input, "\0"), &parsedCmd[0]);

        if(strcmp(input, "exit") == 0){
            exit(0);
        }

        executeCmd(inputlength, &parsedCmd[0], &activeProcesses);

        freeParsed(inputlength, parsedCmd);
        free(input);  
    }
    return 0;
}
