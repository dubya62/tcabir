
#include <stdlib.h>
#include <string.h>

#include "HashMap.h"
#include "ArrayList.h"
#include "debug.h"

typedef enum HashMapStatuses{
    EMPTY,
    DELETED,
    FILLED,
} HashMapStatuses;


#define NUMBER_OF_PRIMES 13
int PRIMES[] = {233, 991, 3709, 16111, 61297, 221987, 892049, 3196507, 15266369, 56070719, 245156719, 1056272471, 4038711613};


// create a HashMap
HashMap* HashMap_malloc(size_t keySize, size_t valueSize, size_t (*prehashFunction)(void*), int (*compareFunction)(void*, void*)){
    HashMap* result = (HashMap*) malloc(sizeof(HashMap));

    result->prehashFunction = prehashFunction;
    result->compareFunction = compareFunction;

    result->currentPrime = 0;
    result->size = 0;
    result->space = PRIMES[result->currentPrime];

    result->keySize = keySize;
    result->valueSize = valueSize;

    result->status = (char*) malloc(sizeof(char) * result->space);
    for (int i=0; i < result->space; i++){
        result->status[i] = (char) EMPTY;
    }

    result->keys = malloc(keySize * result->space);
    result->values = malloc(valueSize * result->space);

    return result;
}


// free a HashMap's memory
void HashMap_free(HashMap* instance){
    free(instance->status);
    free(instance->keys);
    free(instance->values);
}

HashMap* HashMap_copy(HashMap* instance){
    HashMap* result = (HashMap*) malloc(sizeof(HashMap));

    result->currentPrime = instance->currentPrime;
    result->size = instance->size;
    result->space = instance->space;

    result->prehashFunction = instance->prehashFunction;
    result->compareFunction= instance->compareFunction;

    result->keySize = instance->keySize;
    result->valueSize = instance->valueSize;

    result->status = (char*) malloc(sizeof(char) * instance->space);
    result->keys = (char*) malloc(result->keySize * instance->space);
    result->values = (char*) malloc(result->valueSize * instance->space);

    memcpy(result->status, instance->status, sizeof(char) * instance->space);
    memcpy(result->keys, instance->keys, result->keySize * instance->space);
    memcpy(result->values, instance->values, result->valueSize * instance->space);

    return result;
}


// get the number of elements in a HashMap
int HashMap_size(HashMap* instance){
    return instance->size;
}


// hash a key
size_t hashKey(size_t keySize, void* key){
    char* charKey = (char*) key;
    size_t result = 0;

    for (int i=0; i<keySize; i++){
        result += charKey[i];
    }

    return result;
}


// rehash a HashMap
void HashMap_rehash(HashMap* instance){
    if (instance->currentPrime >= NUMBER_OF_PRIMES - 1){
        panic("Internal Error. Attempted to create a HashMap that uses too much memory.\n");
    }


    HashMap* newMap = (HashMap*) malloc(sizeof(HashMap));
    newMap->prehashFunction = instance->prehashFunction;
    newMap->compareFunction = instance->compareFunction;
    newMap->currentPrime = instance->currentPrime + 1;
    newMap->size = 0;
    newMap->space = PRIMES[instance->currentPrime];

    newMap->status = (char*) malloc(sizeof(char) * newMap->space);
    for (int i=0; i<newMap->space; i++){
        newMap->status[i] = (char) EMPTY;
    }
    newMap->keys = malloc(instance->keySize * newMap->space);
    newMap->values = malloc(instance->valueSize * newMap->space);

    char* instanceCharKeys = (char*) instance->keys;
    char* instanceCharValues = (char*) instance->values;

    for (int i=0; i<instance->size; i++){
        if ((int) (instance->status[i]) == FILLED){
            HashMap_put(newMap, instanceCharKeys+(i * instance->keySize), instanceCharValues+(i * instance->valueSize));
        }
    }

    // copy the data from the new map to the old one
    instance->space = newMap->space;
    instance->currentPrime = newMap->currentPrime;
    free(instance->status);
    free(instance->keys);
    free(instance->values);
    instance->status = newMap->status;
    instance->keys = newMap->keys;
    instance->values = newMap->values;
    free(newMap);
}


// add a value to a hashmap (return 1 if it was already there)
int HashMap_put(HashMap* instance, void* key, void* value){
    // if size/space > .7, rehash
    float filled = (float) instance->size / instance->space;
    if (filled > .7){
        HashMap_rehash(instance);
    }

    // get the prehash value of the key
    size_t prehash;
    if (instance->prehashFunction == NULL){
        prehash = hashKey(instance->keySize, key);
    } else {
        prehash = (*(instance->prehashFunction))(key);
    }
    size_t hash = prehash % instance->space;

    // use linear probing to find a valid spot
    char* charKeys = (char*) instance->keys;
    char* charValues = (char*) instance->values;

    while (1){
        if (instance->status[hash] == FILLED){
            // continue probing
            if (instance->compareFunction == NULL){
                if (!memcmp(key, charKeys + (hash * instance->keySize), instance->keySize)){
                    return 1;
                }
            } else {
                // use compare function if one was given
                if ((*(instance->compareFunction))(key, charKeys + (hash * instance->keySize))){
                    memcpy(charValues + (hash * instance->valueSize), value, instance->valueSize);
                    return 1;
                }
            }
            hash++;
            hash %= instance->space;
        } else {
            // place right here
            instance->status[hash] = FILLED;
            memcpy(charKeys + (hash * instance->keySize), key, instance->keySize);
            memcpy(charValues + (hash * instance->valueSize), value, instance->valueSize);
            instance->size++;
            break;
        }
    }
    return 0;
}

// get a value from a hashmap by key
void* HashMap_get(HashMap* instance, void* key){
    // get the prehash value of the key
    size_t prehash;
    if (instance->prehashFunction == NULL){
        prehash = hashKey(instance->keySize, key);
    } else {
        prehash = (*(instance->prehashFunction))(key);
    }
    size_t hash = prehash % instance->space;
    size_t lastCheck = hash - 1;
    if (lastCheck < 0){
        lastCheck = instance->space-1;
    }

    // use linear probing to find a valid spot
    char* charKeys = (char*) instance->keys;
    char* charValues = (char*) instance->values;

    while (1){
        if (instance->status[hash] == EMPTY || hash == lastCheck){
            return NULL;
        }

        if (instance->compareFunction == NULL){
            if (!memcmp(key, charKeys + (hash * instance->keySize), instance->keySize)){
                return charValues + (hash * instance->valueSize);
            }
        } else {
            if ((*(instance->compareFunction))(key, charKeys + (hash * instance->keySize))){
                return charValues + (hash * instance->valueSize);
            }
        }

        hash++;
        hash %= instance->space;

    }

}

// remove a value from a hashmap by key if it exists (returns 1 if it existed)
int HashMap_remove(HashMap* instance, void* key){
    char* location = (char*) HashMap_get(instance, key);
    if (location == NULL){
        return 0;
    }

    int index = (location - ((char*) instance->values)) / instance->valueSize;
    instance->status[index] = DELETED;
    instance->size--;
    return 1;
}


// see if the HashMap contains a certain key
int HashMap_containsKey(HashMap* instance, void* key){
    return HashMap_get(instance, key) != NULL;
}

char* selfToString(void* self){
    return (char*) self;
}

// convert a HashMap to a String given two toString functions
char* HashMap_toString(HashMap* instance, char* keyToStringFunc(void*), char* valueToStringFunc(void*)){
    ArrayList* resultList = ArrayList_malloc(sizeof(char*));

    if (keyToStringFunc == NULL){
        keyToStringFunc = selfToString;
    }
    if (valueToStringFunc == NULL){
        valueToStringFunc = selfToString;
    }

    ArrayList_append(resultList, "{");
    for (int i=0; i<instance->space; i++){
        if ((int) instance->status[i] == FILLED){
            ArrayList_append(resultList, keyToStringFunc(instance->keys + (i * instance->keySize)));
            ArrayList_append(resultList, ":");
            ArrayList_append(resultList, valueToStringFunc(instance->values+ (i * instance->valueSize)));
            ArrayList_append(resultList, ", ");
        }
    }
    ArrayList_append(resultList, "}");

    char* result = ArrayList_toOnlyString(resultList, selfToString);

    ArrayList_free(resultList);
    return result;
}


