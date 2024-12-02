
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "ArrayList.h"

// the ArrayList size is 1<<DEFAULT_ARRAYLIST_SIZE
#define DEFAULT_ARRAYLIST_SIZE 10 

// create a single ArrayBlock
ArrayBlock* ArrayBlock_malloc(size_t nmemb, size_t size){
    ArrayBlock* result = (ArrayBlock*) malloc(sizeof(ArrayBlock));

    result->length = 0;
    result->size = nmemb;
    result->data = malloc(nmemb * size);

    return result;
}

// create an empty ArrayList and return it
ArrayList* ArrayList_malloc(size_t memberSize){
    ArrayList* result = (ArrayList*) malloc(sizeof(ArrayList));

    result->length = 0;
    result->numberOfBlocks = 1;
    result->memberSize = memberSize;

    result->blocks = (ArrayBlock*) malloc(sizeof(ArrayBlock));
    ArrayBlock* currentBlock = ArrayBlock_malloc(1<<DEFAULT_ARRAYLIST_SIZE, memberSize);
    result->blocks[0] = *(currentBlock);
    free(currentBlock);

    return result;
}
// free ArrayList memory
void ArrayBlock_free(ArrayBlock* instance){
    free(instance->data);
}
void ArrayList_free(ArrayList* instance){
    for (int i=0; i<instance->numberOfBlocks; i++){
        ArrayBlock_free(instance->blocks + i);
    }
    free(instance->blocks);
    free(instance);
}

// get number of elements in an ArrayList
int ArrayList_length(ArrayList* instance){
    return instance->length;
}
// append an element to the ArrayList
void ArrayList_append(ArrayList* instance, void* value){
    // check the last block to see if it is full
    ArrayBlock* lastBlock = instance->blocks + (instance->numberOfBlocks-1);
    if (lastBlock->length == lastBlock->size){
        // we must allocate a new block
        ArrayBlock* newBlocks = (ArrayBlock*) malloc(sizeof(ArrayBlock) * (instance->numberOfBlocks+1));


        memcpy(newBlocks, instance->blocks, instance->numberOfBlocks * sizeof(ArrayBlock));

        free(instance->blocks);
        instance->blocks = newBlocks;

        // double the size for each new block
        ArrayBlock* currentBlock = ArrayBlock_malloc(1<<(DEFAULT_ARRAYLIST_SIZE + instance->numberOfBlocks), instance->memberSize);
        instance->blocks[instance->numberOfBlocks] = *(currentBlock);
        lastBlock = instance->blocks + (instance->numberOfBlocks);

        instance->numberOfBlocks++;
    } 

    // copy the value over
    char* charData = (char*) lastBlock->data;
    memcpy(charData + (lastBlock->length * instance->memberSize), value, instance->memberSize);
    instance->length++;
    lastBlock->length++;
}

// return which block an index lies in or -1 if it does not exist
int findBlockIndex(ArrayList* instance, size_t index, int* withinIndex){
    for (int i=0; i<instance->numberOfBlocks; i++){
        if (index < instance->blocks[i].length){
            *withinIndex = index;
            return i;
        }
        index -= instance->blocks[i].length;
    }
    return -1;
}

// insert an element into the ArrayList
void ArrayList_insert(ArrayList* instance, size_t index, void* value){
    // find the block it goes in
    int withinIndex;
    int blockIndex = findBlockIndex(instance, index, &withinIndex);
    if (blockIndex == -1){
        // simply append if the index is too large
        ArrayList_append(instance, value);
    } else {
        // if there is still space in this block, move everything to the right once
        // else, go to next block
        // if next block has space, move everything to the right once
        int currentBlock = blockIndex;
        while (1){
            if (instance->blocks[currentBlock].size > instance->blocks[currentBlock].length){
                // this block is valid
                int first = 1;
                char* blockData;
                while (currentBlock != blockIndex){
                    blockData = (char*) (instance->blocks[currentBlock].data);
                    if (!first){
                        memcpy(instance->blocks[currentBlock+1].data, blockData + (instance->blocks[currentBlock].length-1), instance->memberSize);
                        instance->blocks[currentBlock+1].length++;
                        instance->blocks[currentBlock].length--;
                    } 
                    memmove(blockData + instance->memberSize, blockData, instance->blocks[currentBlock].length * instance->memberSize);
                    currentBlock--;
                    first = 0;
                }
                blockData = (char*) (instance->blocks[currentBlock].data);
                memmove(blockData + (withinIndex+1) * instance->memberSize, blockData + withinIndex * instance->memberSize, (instance->blocks[currentBlock].length - withinIndex) * instance->memberSize);
                memcpy(blockData + withinIndex * instance->memberSize, value, instance->memberSize);
                instance->blocks[currentBlock].length++;
                instance->length++;
                break;
            } else {
                currentBlock++;
            }
        }

        instance->length++;
    }
}

// return the value at a given index
void* ArrayList_get(ArrayList* instance, size_t index){
    int withinIndex;
    int theIndex = findBlockIndex(instance, index, &withinIndex);

    return ((char*) instance->blocks[theIndex].data) + (withinIndex * instance->memberSize);
}

// remove a value from an ArrayList
void ArrayList_remove(ArrayList* instance, size_t index){
    int withinIndex;
    int theIndex = findBlockIndex(instance, index, &withinIndex);
    int moveSize = instance->blocks[theIndex].length -  withinIndex;
    if (moveSize > 0){
        char* removeIndex = ((char*) (instance->blocks[theIndex].data)) + (instance->memberSize * withinIndex);
        memmove(removeIndex, removeIndex + instance->memberSize, instance->memberSize * moveSize);
    }
    instance->blocks[theIndex].length--;
    instance->length--;
}

// return the string representation of an ArrayList given a toString() function
char* ArrayList_toString(ArrayList* instance, char* toStringFunc(void*)){
    ArrayList* charBuffer = ArrayList_malloc(sizeof(char));

    char* currentValue;
    int k;
    ArrayList_append(charBuffer, "[");
    for (int i=0; i<instance->numberOfBlocks; i++){
        char* charData = (char*) (instance->blocks[i].data);
        for (int j=0; j<instance->blocks[i].length; j++){
            currentValue = toStringFunc(charData + (j * instance->memberSize));
            k = 0;
            while (1){
                if (currentValue[k] == '\0'){
                    ArrayList_append(charBuffer, ",");
                    ArrayList_append(charBuffer, " ");
                    break;
                }
                ArrayList_append(charBuffer, currentValue+k);
                k++;
            }
        }
    }
    ArrayList_append(charBuffer, "]");


    size_t finalBufferLength = charBuffer->length * charBuffer->memberSize + 1;
    char* finalBuffer = (char*) malloc(finalBufferLength);

    // concatenate the characters into a single string
    k = 0;
    for (int i=0; i<charBuffer->numberOfBlocks; i++){
        char* charBufferData = (char*) (charBuffer->blocks[i].data);
        for (int j=0; j<charBuffer->blocks[i].length; j++){
            finalBuffer[k] = charBufferData[j];
            k++;
        }
    }

    ArrayList_free(charBuffer);

    finalBuffer[finalBufferLength-1] = '\0';
    return finalBuffer;
}

char* ArrayList_toOnlyString(ArrayList* instance, char* toStringFunc(void*)){
    ArrayList* charBuffer = ArrayList_malloc(sizeof(char));

    char* currentValue;
    int k;
    for (int i=0; i<instance->numberOfBlocks; i++){
        char* charData = (char*) (instance->blocks[i].data);
        for (int j=0; j<instance->blocks[i].length; j++){
            currentValue = toStringFunc(charData + (j * instance->memberSize));
            k = 0;
            while (1){
                if (currentValue[k] == '\0'){
                    break;
                }
                ArrayList_append(charBuffer, currentValue+k);
                k++;
            }
        }
    }

    size_t finalBufferLength = charBuffer->length * charBuffer->memberSize + 1;
    char* finalBuffer = (char*) malloc(finalBufferLength);

    // concatenate the characters into a single string
    k = 0;
    for (int i=0; i<charBuffer->numberOfBlocks; i++){
        char* charBufferData = (char*) (charBuffer->blocks[i].data);
        for (int j=0; j<charBuffer->blocks[i].length; j++){
            finalBuffer[k] = charBufferData[j];
            k++;
        }
    }

    ArrayList_free(charBuffer);

    finalBuffer[finalBufferLength-1] = '\0';
    return finalBuffer;
}

void ArrayList_empty(ArrayList* instance){
    instance->length = 0;
    for (int i=0; i<instance->numberOfBlocks; i++){
        instance->blocks[i].length = 0;
    }
}

