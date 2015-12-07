#include <stdio.h>
#include <stdlib.h>
#include "Set.h"

struct Set {
    long size;
    long alloc;
    void** items;
};

Set Set_toSet(void** items, long size) {
    Set s = malloc(sizeof(struct Set));
    s->size  = size;
    s->alloc = size;
    s->items = items;
    return s;
}

Set Set_new() {
    return Set_toSet(NULL, 0);
}

void** Set_toArray(Set s, long* newSize) {
    if (s == NULL)
        return NULL;
    void** it = s->items;
    void** all = calloc(Set_size(s), sizeof(void*));
    long i;
    for (i = 0; i < Set_size(s); i++)
        all[i] = it;
    *newSize = Set_size(s);
    return all;
}

long Set_size(Set s) {
    return s == NULL ? -1 : s->size;
}

bool Set_has(Set s, void* item) {
    if (s == NULL || item == NULL) return false;
    long i;
    for (i = 0; i < Set_size(s); i++)
        if (s->items[i] == item)
            return true;
    return false;
}

Set Set_add(Set s, void* item) {
    if (s == NULL || Set_has(s, item)) return NULL;
    if (s->alloc <= Set_size(s)) {
        void** it = realloc(s->items, Set_size(s) + 1);
        if (it == NULL) return NULL;
        s->items = it;
    }
    s->items[Set_size(s)] = item;
    s->size++;
    return s;
}

Set Set_remove(Set s, void* item) {
    if (s == NULL || !Set_has(s, item)) return NULL;
    long i, j;
    for (i = 0; i < Set_size(s); i++)
        if (s->items[i] == item) {
            s->items[i] = NULL;
            for (j = i; j < Set_size(s); j++)
                if (j != (Set_size(s)-1))
                    s->items[j] = s->items[j+1];
            s->size--;
            return s;
        }
    return s;
}

void Set_clear(Set s) {
    long i = 0;
    for (i = Set_size(s); i > 0; i--) {
        Set_remove(s, s->items[i-1]);
    }
}

void Set_show(Set s, FILE *file, void(*show_func)(FILE*, void*)) {
    fprintf(file, "[");
    long i;
    for (i = 0; i < Set_size(s); i++) {
        if (i > 0)
            fprintf(file, ", ");
        show_func(file, s->items[i]);
    }
    fprintf(file, "]\n");
}