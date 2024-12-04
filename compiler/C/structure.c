
#include <stdlib.h>

#include "HashMap.h"

#include "structure.h"
#include "token.h"

// allocate memory for a new structure
Structure* Structure_malloc(){
    Structure* result = (Structure*) malloc(sizeof(Structure));

    result->members = HashMap_malloc(sizeof(char*), sizeof(char*), &stringPrehashFunction, &stringCompareFunction); // name:type

    return result;
}

// deallocate memory for a structure
void Structure_free(Structure* instance){
    HashMap_free(instance->members);
}

// add a new member based on type and name (return 1 if already existed)
int Structure_add(Structure* instance, char* name, char* type){
    return HashMap_put(instance->members, &name, &type);
}

