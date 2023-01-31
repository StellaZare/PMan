#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

/* Prints current working directory to stdout */
void printDirectory() {
    char directory [1000];
    if ( getcwd(directory, sizeof(directory)) != NULL){
        printf("Current Directory: %s\n", directory);
    }else{
        printf("Error in getcwd() call\n");
    }
   
}

int main(){
    printDirectory();
    int run = 1;
    char* input;

    while(run){
        input = readline("PMan: > ");
        if (strlen(input) == 0)
            continue;
        add_history(input);
        //parseInput(input);

        if (strcmp(input, "exit") == 0)
            run = 0;
        free(input);
    }
    return 0;
}