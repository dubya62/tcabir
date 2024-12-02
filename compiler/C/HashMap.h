#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct HashMap{
    int currentPrime;

    size_t (*prehashFunction)(void*); // function to hash a given input
    int (*compareFunction)(void*, void*); // function to compare two keys
    
    size_t size; // total filled slots
    size_t space; // max spots
    size_t keySize; // bytes that each key uses
    size_t valueSize; // bytes that each value uses

    char* status; // keep track of key status (0 = nothing, 1 = deleted, 2 = something)
    void* keys; // array to index the keys
    void* values; // array to index the values
} HashMap;


// create a HashMap
HashMap* HashMap_malloc(size_t keySize, size_t valueSize, size_t (*prehashFunction)(void*), int (*compareFunction)(void*, void*));
// free a HashMap's memory
void HashMap_free(HashMap* instance); 
// get the number of elements in a HashMap
int HashMap_size(HashMap* instance);
// add a value to a hashmap (return 1 if it was already there)
int HashMap_put(HashMap* instance, void* key, void* value);
// get a value from a hashmap by key (return NULL if it does not exist)
void* HashMap_get(HashMap* instance, void* key);
// remove a value from a hashmap by key if it exists (returns 1 if it existed)
int HashMap_remove(HashMap* instance, void* key);
// see if the HashMap contains a certain key
int HashMap_containsKey(HashMap* instance, void* key);
// convert a HashMap to a String given two toString functions
char* HashMap_toString(HashMap* instance, char* keyToStringFunc(void*), char* valueToStringFunc(void*));



#endif
