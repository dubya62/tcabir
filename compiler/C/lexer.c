
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "lexer.h"
#include "ArrayList.h"
#include "token.h"

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
    dbg("==============================\n");
    dbg(fileBuffer);
    dbg("==============================\n");

    return fileBuffer;
}


int isBreakChar(char theChar){
    switch(theChar){
        case '~':
        case '!':
        case '#':
        case '%':
        case '^':
        case '&':
        case '*':
        case '(':
        case ')':
        case '-':
        case '+':
        case '=':
        case '{':
        case '}':
        case '[':
        case ']':
        case '|':
        case '\'':
        case '"':
        case ';':
        case ':':
        case '/':
        case '?':
        case '.':
        case ',':
        case '<':
        case '>':
        case '\n':
        case '\t':
        case '\\':
        case ' ':
            return 1;
    }
    return 0;
}


// break a string up into tokens based on the break characters
ArrayList* breakIntoTokens(char* fileContents){
    ArrayList* result = ArrayList_malloc(sizeof(Token));

    int i = 0;

    char* currentString = fileContents;
    int currentLength = 0;

    int currentLineNumber = 1;

    while (1){
        if (fileContents[i] == '\0'){
            break;
        }

        if (isBreakChar(fileContents[i])){

            if (fileContents[i] == '\n'){
                currentLineNumber++;
            }

            Token* newToken;
            if (currentLength > 0){
                char* stringCopy = (char*) malloc(sizeof(char) * (currentLength+1));
                memcpy(stringCopy, currentString, sizeof(char) * currentLength);
                stringCopy[currentLength] = '\0';
                // create a new Token for the current string
                newToken = stringToToken(stringCopy);
                newToken->lineNumber = currentLineNumber;
                ArrayList_append(result, newToken);
            }
            // create another new token for the current break character
            char* breakCharCopy = (char*) malloc(sizeof(char) * 2);
            breakCharCopy[0] = fileContents[i];
            breakCharCopy[1] = '\0';
            newToken = stringToToken(breakCharCopy);
            newToken->lineNumber = currentLineNumber;
            ArrayList_append(result, newToken);

            // empty the current string
            currentString = fileContents+i+1;
            currentLength = 0;
        } else {
            currentLength++;
        }

        i++;
    }


    return result;
}




///////////////////////////////////////////////////
// public functions

ArrayList* performLexicalAnalysis(char* filename){

    // get the file contents
    dbg("Reading the input file...\n");
    char* fileContents = readFile(filename);

    // break the file up into tokens
    ArrayList* fileTokens = breakIntoTokens(fileContents);

    // update the filename of each token
    for (int i=0; i<ArrayList_length(fileTokens); i++){
        Token* current = (Token*) (ArrayList_get(fileTokens, i));
        current->filename = filename;
    }

    // print debug information
    char numberOfTokens[1024];
    snprintf(numberOfTokens, 1024, "%d", fileTokens->length);
    dbg("\n");
    dbg("Lexical Analysis Generated "); dbg(numberOfTokens); dbg(" Tokens!\n");
    dbg("Output of Lexical analysis:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(fileTokens, Token_toString));
    dbg("\n==============================\n");
    dbg("\n");

    free(fileContents);
    return fileTokens;
}
