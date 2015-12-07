#include "Set.h"
#include <assert.h>

void show_func(FILE* file, void* it) {
     fprintf(file, "%d", *(int*)it);
}

int main(int argc, char* argv[]) {
    Set s = Set_new();
    int   a = 42;
    char* b = "hello";
    assert(Set_size(s) == 0);
    Set_show(s, stdout, &show_func);
    Set_add(s, &a);
    assert(Set_size(s) == 1);
    Set_show(s, stdout, &show_func);
    Set_add(s, b);
    assert(Set_size(s) == 2);
    Set_show(s, stdout, &show_func);
    Set_remove(s, b);
    assert(Set_size(s) == 1);
    Set_show(s, stdout, &show_func);
    Set_remove(s, b);
    assert(Set_size(s) == 1);
    Set_show(s, stdout, &show_func);

    fprintf(stdout, "All tests passed!\n");
}