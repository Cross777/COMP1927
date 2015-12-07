#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int factorial(int);

int main(int argc, char *argv[]) {
   if (argc != 2) {
      fprintf(stderr, "Usage: %s number\n", argv[0]);
      return EXIT_FAILURE;
   }
   
   int fac = factorial(atoi(argv[1]));
   printf("Factorial = %d\n", fac);
   
   return EXIT_SUCCESS;
}

int factorial(int num) {
   if (num == 1) {
      return num;
   } else {
      return num * factorial(num-1);
   }
}
