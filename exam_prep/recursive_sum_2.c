#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sum_recursive(int, int);

int main(int argc, char *argv[]) {
   if (argc != 2) {
      fprintf(stderr, "Usage: %s number\n", argv[0]);
      return EXIT_FAILURE;
   }
   
   int sum = sum_recursive(atoi(argv[1]), 0);
   printf("Recursive sum = %d\n", sum);
   
   return EXIT_SUCCESS;
}

int sum_recursive(int num, int counter) {
   if (counter == num) {
      return num;
   } else {
      return counter + sum_recursive(num, counter+1);
   }
}
