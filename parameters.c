#include <stdio.h>

int main(int argc, char** argv) {
  
  int i;

  printf("Number of arguments: %d\n", argc - 1);
  printf("Arguments: \n");

  for (i = 1; i < argc; i++) {
    printf("%d. %s\n", i, argv[i]);
  }

  return 0;
}