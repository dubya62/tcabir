/*
* The purpose of this program is to act as a compiler
* to convert from simple C to the tcab Internal Representation.
*
* The Internal Represenation's goal is to allow substituting code
* that is more efficient by some metric.
*
* After all substitutions and optimizations are finished, 
* convert the code back to C or Cuda and compile using the respective
* compiler.
*/

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "lexer.h"

#include "ArrayList.h"


// get the filename to compile or throw an error
int handleCommandLineArguments(int argc, char** argv, char** filename){
    if (argc < 1){
        printf("Please Supply A Filename to Compile...\n");
        return 1;
    }

    *filename = argv[1];

    return 0;
}

char* charToString(void* c){
    char* result = malloc(sizeof(char) * 2);
    result[0] = ((char*)c)[0];
    result[1] = '\0';
    return result;
}

int main(int argc, char** argv){
    dbg("Hello, World!\n");

    // handle the command line arguments
    char* filename;

    dbg("Parsing the Command Line Arguments...\n");
    int commandLineArgsError = handleCommandLineArguments(argc, argv, &filename);
    if (commandLineArgsError){
        panic("There was an error while handling the command line arguments...\n");
    } else {
        dbg("Command line arguments parsed successfully!\n");
        dbg("Input filename: ");
        dbg(filename);
        dbg("\n");
    }

    dbg("\n");
    dbg("Compiling the input file...\n");

    // lex the file and get the tokens
    dbg("\n");
    dbg("Performing Lexical Analysis...\n");
    char** tokens = performLexicalAnalysis(filename);
    dbg("Lexical Analysis Finshed!\n");

    dbg("\n");


    return 0;
}
