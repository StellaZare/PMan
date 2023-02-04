#include<stdio.h>
#include <unistd.h>

int main(){
    int reps = 10;

    for (int i = 0; i < reps; i++){
        //printf("rep %d\n", i);
        sleep(3);
    }

    printf("Background process finished.\n");

    return  0;
}