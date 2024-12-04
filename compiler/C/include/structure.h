#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "ArrayList.h"

typedef struct Structure{
    HashMap* members; // membername:membertype - char*:char*
} Structure;

// allocate memory for a new structure
Structure* Structure_malloc();
// deallocate memory for a structure
void Structure_free(Structure* instance);
// add a new member based on type and name
int Structure_add(Structure* instance, char* name, char* type);

#endif
