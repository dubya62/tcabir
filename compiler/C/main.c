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

#include "ArrayList.h"
#include "HashMap.h"

#include "debug.h"
#include "lexer.h"
#include "token.h"
#include "normalizer.h"
#include "builtins.h"
#include "simplifier.h"

HashMap* BUILTIN_FUNCTIONS;
HashMap* OPERATORS;
HashMap* OPERATOR_PRECEDENCES;
HashMap* DELIMITERS;

HashMap* TYPES;
HashMap* VARIABLE_TYPES;
HashMap* STATEMENTS;

// get the filename to compile or throw an error
int handleCommandLineArguments(int argc, char** argv, char** filename){
    if (argc < 2){
        printf("Please Supply A Filename to Compile...\n");
        return 1;
    }

    *filename = argv[1];

    return 0;
}


char* intToString(void* value){
    return (char*) value;
}

int main(int argc, char** argv){
    dbg("Hello, World!\n");


    dbg("##################################################\n");

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

    initBuiltins();

    dbg("\n");
    dbg("Compiling the input file...\n");

    dbg("##################################################\n");

    // lex the file and get the tokens
    dbg("\n");
    dbg("Performing Lexical Analysis...\n");
    ArrayList* tokens = performLexicalAnalysis(filename);
    dbg("Lexical Analysis Finshed!\n");

    dbg("##################################################\n");

    // normalize the tokens in the file to a simpler representation
    dbg("\n");
    dbg("Performing basic normlization...\n");
    tokens = performBasicNormalization(tokens);
    dbg("Basic Normalization Finished!\n");

    dbg("##################################################\n");

    dbg("\n");
    dbg("Performing Variable Simplification...\n");
    tokens = performVariableSimplification(tokens);
    dbg("Variable Simplification Finished!\n");


    // memory cleanup
    freeBuiltins();
    return 0;
}
