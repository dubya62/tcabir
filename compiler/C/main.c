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
#include <string.h>

#include "ArrayList.h"
#include "HashMap.h"

#include "debug.h"
#include "lexer.h"
#include "token.h"
#include "normalizer.h"
#include "builtins.h"
#include "simplifier.h"
#include "preprocessor.h"

HashMap* BUILTIN_FUNCTIONS;
HashMap* OPERATORS;
HashMap* OPERATOR_PRECEDENCES;
HashMap* DELIMITERS;

HashMap* TYPES;
HashMap* VARIABLE_TYPES;
HashMap* STATEMENTS;
HashMap* VARIABLE_NAMES;

char* outputFilename = "a.out";
ArrayList* includeDirs;
ArrayList* inputFiles;
int compileToObject;

/*
 * -h       help information
 * -o       output filename
 * -I       include directories
 * -c       compile to object
 */

// get the filename to compile or throw an error
int handleCommandLineArguments(int argc, char** argv){
    if (argc < 2){
        panic("fatal error: no input files\n");
    }

    compileToObject = 0;

    includeDirs = ArrayList_malloc(sizeof(char*));
    inputFiles = ArrayList_malloc(sizeof(char*));

    int argLength;
    for (int i=1; i<argc; i++){
        argLength = strlen(argv[i]);
        if (!strcmp(argv[i], "-o")){
            if (i + 1 >= argc){
                printf("error: missing filename after -o\n");
                exit(1);
            }
        } else if (argLength > 2 && argv[i][0] == '-' && argv[i][1] == 'o'){
            outputFilename = argv[i] + 2;
        } else if (!strcmp(argv[i], "-h")){
            printf("Tcabir compiler help:\n");
            printf("\t./tcabcc [options] [input files]\n");
            printf("\t-h                print help information\n");
            printf("\t-o [filename]     \n");
            printf("\t-I [directory]    add include directory\n");
            printf("\t-c                compile to object file\n");
            exit(0);
        } else if (!strcmp(argv[i], "-I")){
            if (i + 1 >= argc){
                printf("error: missing path after -I\n");
                exit(1);
            }
        } else if (argLength > 2 && argv[i][0] == '-' && argv[i][1] == 'I'){
            char** includePointer = (char**) malloc(sizeof(char*));
            *(includePointer) = (argv[i] + 2);
            ArrayList_append(includeDirs, includePointer);
        } else if (!strcmp(argv[i], "-c")){
            compileToObject = 1;
        } else if (argLength > 0 && argv[i][0] == '-'){
            printf("error: unrecognized command-line option\n");
            printf("use -h for help\n");
            exit(0);
        } else {
            ArrayList_append(inputFiles, argv+i);
        }
    }

    if (!ArrayList_length(inputFiles)){
        printf("fatal error: no input files\n");
        exit(1);
    }

    return 0;
}


char* intToString(void* value){
    return (char*) value;
}

int main(int argc, char** argv){
    dbg("Hello, World!\n");


    dbg("##################################################\n");

    // Print information gained from command line arguments
    dbg("Parsing the Command Line Arguments...\n");
    int commandLineArgsError = handleCommandLineArguments(argc, argv);
    dbg("Command line arguments parsed successfully!\n");
    dbg("Input filenames: ");
    dbg(ArrayList_toString(inputFiles, NULL));
    dbg("\n");
    dbg("Output filename: ");
    dbg(outputFilename);
    dbg("\n");
    dbg("Included directories: ");
    dbg(ArrayList_toString(includeDirs, NULL));
    dbg("\n");
    dbg("Compiling to object: ");
    dbg(compileToObject ? "true" : "false");
    dbg("\n");

    initBuiltins();

    char* filename = *((char**) ArrayList_get(inputFiles, 0));

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

    // handle compiler directives
    HashMap* includedAlready = HashMap_malloc(sizeof(char*), sizeof(char), stringPrehashFunction, stringCompareFunction); // keep track of filename that have already been included
    dbg("\n");
    dbg("Handling Compiler Directives...\n");
    tokens = handleCompilerDirectives(tokens, includeDirs, includedAlready);
    dbg("Compiler Directives Handled!\n");

    dbg("##################################################\n");


    dbg("\n");
    dbg("Performing Variable Simplification...\n");
    tokens = performVariableSimplification(tokens);
    dbg("Variable Simplification Finished!\n");


    // memory cleanup
    freeBuiltins();
    return 0;
}
