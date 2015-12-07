// useIntList.c - testing IntList data type

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "IntList.h"

int main(int argc, char *argv[])
{
	IntList myList, myOtherList;

	myList = getIntList(stdin);
	// showIntList(myList); // comment out this...
	assert(IntListOK(myList));

	myOtherList = IntListSortedCopy(myList);
	// printf("Sorted:\n"); // ...and this for Task 2
	showIntList(myOtherList);
	assert(IntListOK(myOtherList));
	assert(IntListIsSorted(myOtherList));

	return 0;
}
