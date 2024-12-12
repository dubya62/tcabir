
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

typedef struct TestStruct{
    int a;
    int b;
} TestStruct;

struct TestStruct* foo(int a){
    return NULL;
}

int main(int argc, char** argv){
    TestStruct test;
    test.a = (unsigned int) 2;
    test.b = 4;
}

