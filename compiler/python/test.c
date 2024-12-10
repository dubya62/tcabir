
#include <stdio.h>

int testFunction(void* arg1){
    char* charData = (char*) arg1;
    printf("%s\n", charData);
    return 0;
}
