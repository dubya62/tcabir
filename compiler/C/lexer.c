
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "lexer.h"
#include "ArrayList.h"

// read a file and return its contents
char* readFile(char* filename){
    FILE* theFile = fopen(filename, "r");

    if (theFile == NULL){
        panic("ERROR: Could not open the input file.\n");
    }

    dbg("Input file opened successfully!\n");

    // go to end of file and get position to get size
    fseek(theFile, 0, SEEK_END);
    long fileLength = ftell(theFile);

    // allocate buffer to store file contents
    char* fileBuffer = (char*) malloc(sizeof(char) * (fileLength + 1));

    // go back to the start of the file and read it into the buffer
    fseek(theFile, 0, SEEK_SET);
    size_t bytesWritten = fread(fileBuffer, sizeof(size_t), fileLength/sizeof(size_t), theFile) * sizeof(size_t);

    int remainingBytes = fileLength % sizeof(size_t);
    if (remainingBytes != 0){
        fread(fileBuffer + bytesWritten, sizeof(char), remainingBytes, theFile);
    }
    fclose(theFile);

    fileBuffer[(size_t) fileLength] = '\0';

    dbg("File has successfully been read into memory...\n");
    dbg("File contents:\n");
    dbg("==============\n");
    dbg(fileBuffer);
    dbg("==============\n");

    return fileBuffer;
}














///////////////////////////////////////////////////
// public functions

char** performLexicalAnalysis(char* filename){

    // get the file contents
    dbg("Reading the input file...\n");
    char* fileContents = readFile(filename);

    // break the file up into tokens
    

    return NULL;
}
