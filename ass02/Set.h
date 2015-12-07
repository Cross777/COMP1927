#include <stdbool.h>
#include <stdio.h>

typedef struct Set *Set;

Set Set_new();

Set Set_toSet(void** items, long size);

void** Set_toArray(Set s, long* newSize);

long Set_size(Set s);

bool Set_has(Set s, void* item);

Set Set_add(Set s, void* item);

Set Set_remove(Set s, void* item);

void Set_clear(Set s);

void Set_show(Set s, FILE* file, void(*show_func)(FILE*, void*));