#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUMBERS 10 // how many numbers to be summed up

int sum_iterative(int*);
int sum_recursive_v1(int*, int);
int sum_recursive_v2(int*, int);

int main(int argc, char *argv[]) {
   // +1 as argc includes program name, stored in argv[0]
   if (argc != MAX_NUMBERS+1) {
      fprintf(stderr, "Usage: %s (%d space-separated numbers)\n", argv[0], MAX_NUMBERS);
      return EXIT_FAILURE;
   }
   
   int i;
   int nums[MAX_NUMBERS];
   for (i = 1; i <= MAX_NUMBERS; i++) {
      nums[i-1] = atoi(argv[i]);
   }
   
   int sum_i = sum_iterative(nums);
   printf("Iterative sum = %d\n", sum_i);
   
   int sum_r1 = sum_recursive_v1(nums, 0);
   printf("Recursive sum (1) = %d\n", sum_r1);
   
   int sum_r2 = sum_recursive_v2(nums, MAX_NUMBERS-1);
   printf("Recursive sum (2) = %d\n", sum_r2);
   
   return EXIT_SUCCESS;
}

int sum_iterative(int *nums) {
   int i, sum = 0;
   for (i = 0; i < MAX_NUMBERS; i++) {
      sum += nums[i];
   }
   return sum;
}

int sum_recursive_v1(int *nums, int index) {
   if (index == MAX_NUMBERS-1) {
      return nums[index];
   } else {
      return nums[index] + sum_recursive_v1(nums, index+1);
   }
}

int sum_recursive_v2(int *nums, int index) {
   if (index == 0) {
      return nums[0];
   } else {
      return nums[index] + sum_recursive_v2(nums, index-1);
   }
}
