#ifndef ARRAYLIST_H
#define ARRAYLIST_H

typedef struct ArrayBlock{
    size_t length; // number of elements used
    size_t size; // total spaces
    void* data;
} ArrayBlock;

typedef struct ArrayList{
    size_t length;
    size_t numberOfBlocks;
    size_t memberSize;
    ArrayBlock* blocks;
} ArrayList;

// create an empty ArrayList and return it
ArrayList* ArrayList_malloc(size_t memberSize);
// free ArrayList memory
void ArrayList_free(ArrayList* instance);

// get number of elements in an ArrayList
int ArrayList_length(ArrayList* instance);
// append an element to the ArrayList
void ArrayList_append(ArrayList* instance, void* value);
// insert an element into the ArrayList
void ArrayList_insert(ArrayList* instance, size_t index, void* value);
// return the value at a given index
void* ArrayList_get(ArrayList* instance, size_t index);
// remove a value from an ArrayList
void ArrayList_remove(ArrayList* instance, size_t index);
// return the string representation of an ArrayList given a toString() function
char* ArrayList_toString(ArrayList* instance, char* toStringFunc(void*));
char* ArrayList_toOnlyString(ArrayList* instance, char* toStirngFunc(void*));
// empty an ArrayList
void ArrayList_empty(ArrayList* instance);


#endif
