// testList.c - testing DLList data type
// Written by John Shepherd, March 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "DLList.h"

int main(int argc, char *argv[])
{
   // TEST 1: add and delete a single item in empty list
   DLList list1 = newDLList();
   assert(validDLList(list1));
   DLListBefore(list1, "text");
   assert(validDLList(list1));
   assert(!DLListIsEmpty(list1));
   assert(strcmp(DLListCurrent(list1), "text") == 0);
   DLListDelete(list1);
   assert(validDLList(list1));
   assert(DLListIsEmpty(list1));
   freeDLList(list1);
   
   // TEST 2: add and delete a single item in empty list
   DLList list2 = newDLList();
   assert(validDLList(list2));
   DLListAfter(list2, "text");
   assert(validDLList(list2));
   assert(!DLListIsEmpty(list2));
   assert(strcmp(DLListCurrent(list2), "text") == 0);
   DLListDelete(list2);
   assert(validDLList(list2));
   assert(DLListIsEmpty(list2));
   freeDLList(list2);
   
   // TEST 3: add and delete a single item in a list with one node
   DLList list3 = newDLList();
   assert(validDLList(list3));
   DLListBefore(list3, "line1");
   assert(strcmp(DLListCurrent(list3), "line1") == 0);
   assert(validDLList(list3));
   assert(!DLListIsEmpty(list3));
   DLListBefore(list3, "line2");
   assert(strcmp(DLListCurrent(list3), "line2") == 0);
   DLListDelete(list3);
   assert(validDLList(list3));
   assert(!DLListIsEmpty(list3));
   freeDLList(list3);
   
   // TEST 4: add and delete a single item in a list with one node
   DLList list4 = newDLList();
   assert(validDLList(list4));
   DLListAfter(list4, "line1");
   assert(validDLList(list4));
   assert(strcmp(DLListCurrent(list4), "line1") == 0);
   assert(!DLListIsEmpty(list4));
   DLListAfter(list4, "line2");
   assert(strcmp(DLListCurrent(list4), "line2") == 0);
   DLListDelete(list4);
   assert(validDLList(list4));
   assert(!DLListIsEmpty(list4));
   freeDLList(list4);
   
   // TEST 5: add and delete a single item in a list with one node
   DLList list5 = newDLList();
   assert(validDLList(list5));
   DLListBefore(list5, "line1");
   assert(validDLList(list5));
   assert(strcmp(DLListCurrent(list5), "line1") == 0);
   assert(!DLListIsEmpty(list5));
   DLListAfter(list5, "line2");
   assert(strcmp(DLListCurrent(list5), "line2") == 0);
   DLListDelete(list5);
   assert(validDLList(list5));
   assert(!DLListIsEmpty(list5));
   freeDLList(list5);
   
   // TEST 6: add and delete a single item in a list with one node
   DLList list6 = newDLList();
   assert(validDLList(list6));
   DLListAfter(list6, "line1");
   assert(validDLList(list6));
   assert(strcmp(DLListCurrent(list6), "line1") == 0);
   assert(!DLListIsEmpty(list6));
   DLListBefore(list6, "line2");
   assert(strcmp(DLListCurrent(list6), "line2") == 0);
   DLListDelete(list6);
   assert(validDLList(list6));
   assert(!DLListIsEmpty(list6));
   freeDLList(list6);
   
   // TEST 7: add and delete multiple specified nodes
   DLList list7 = newDLList();
   assert(validDLList(list7));
   DLListAfter(list7, "line1");
   DLListAfter(list7, "line2");
   DLListAfter(list7, "line3");
   DLListAfter(list7, "line4");
   DLListAfter(list7, "line5");
   assert(validDLList(list7));
   assert(strcmp(DLListCurrent(list7), "line5") == 0);
   assert(!DLListIsEmpty(list7));
   DLListMoveTo(list7, 3);
   DLListDelete(list7);
   assert(validDLList(list7));
   assert(!DLListIsEmpty(list7));
   assert(strcmp(DLListCurrent(list7), "line4") == 0);
   DLListMoveTo(list7, 1);
   DLListDelete(list7);
   assert(validDLList(list7));
   assert(!DLListIsEmpty(list7));
   assert(strcmp(DLListCurrent(list7), "line2") == 0);
   freeDLList(list7);
   
   // TEST 8: delete entire list with multiple nodes
   DLList list8 = newDLList();
   assert(validDLList(list8));
   DLListAfter(list8, "line1");
   DLListAfter(list8, "line2");
   DLListAfter(list8, "line3");
   DLListAfter(list8, "line4");
   DLListAfter(list8, "line5");
   assert(validDLList(list8));
   assert(!DLListIsEmpty(list8));
   int i;
   for (i = 0; i < 5; i++) {
      DLListDelete(list8);
   }
   assert(validDLList(list8));
   assert(DLListIsEmpty(list8));
   freeDLList(list8);
   
   // TEST 9: calling delete function in empty list
   // i.e. `d, d, d, d, d`
   DLList list9 = newDLList();
   assert(validDLList(list9));
   DLListAfter(list9, "line1");
   DLListAfter(list9, "line2");
   DLListAfter(list9, "line3");
   assert(validDLList(list9));
   assert(!DLListIsEmpty(list9));
   for (i = 0; i < 5; i++) {
      DLListDelete(list9);
   }
   assert(validDLList(list9));
   assert(DLListIsEmpty(list9));
   freeDLList(list9);
   
	return 0;
}
