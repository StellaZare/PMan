#include<stdio.h>
#include <unistd.h>

int main(){
    
    sleep(60*3);
    printf("\n--> background process terminated");
    return 0;
}