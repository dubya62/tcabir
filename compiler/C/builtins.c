
#include <stdlib.h>
#include <string.h>

#include "HashMap.h"

#include "builtins.h"
#include "debug.h"


size_t stringPrehashFunction(void* theString){
    char* stringChars = (char*) theString;

    int len = strlen(theString);
    size_t result = 0;
    for (int i=0; i<len; i++){
        result += (size_t) (stringChars[i]);
    }

    return result;
}


char* returnSelf(void* str){
    return (char*) str;
}

// handle BUILTIN_FUNCTIONS
void initBuiltinFunctions(HashMap* builtinFunctions){
    int integerValue = 0;

    integerValue = MALLOC;
    HashMap_put(builtinFunctions, "malloc", &integerValue);

    integerValue = FREE;
    HashMap_put(builtinFunctions, "free", &integerValue);

    integerValue = READ;
    HashMap_put(builtinFunctions, "read", &integerValue);

    integerValue = WRITE;
    HashMap_put(builtinFunctions, "write", &integerValue);

}

// handle OPERATORS
void initOperators(HashMap* operators){
    int integerValue = 0;

    integerValue = BITWISE_NOT;
    HashMap_put(operators, "~", &integerValue);

    integerValue = LOGICAL_NOT;
    HashMap_put(operators, "!", &integerValue);

    integerValue = HASHTAG;
    HashMap_put(operators, "#", &integerValue);

    integerValue = MOD;
    HashMap_put(operators, "%", &integerValue);

    integerValue = XOR;
    HashMap_put(operators, "^", &integerValue);

    integerValue = BITWISE_AND;
    HashMap_put(operators, "&", &integerValue);

    integerValue = TIMES;
    HashMap_put(operators, "*", &integerValue);

    integerValue = REFERENCE;
    HashMap_put(operators, "ref", &integerValue);

    integerValue = DEREFERENCE;
    HashMap_put(operators, "deref", &integerValue);

    integerValue = OPEN_PARENTHESIS;
    HashMap_put(operators, "(", &integerValue);

    integerValue = CLOSE_PARENTHESIS;
    HashMap_put(operators, ")", &integerValue);

    integerValue = MINUS;
    HashMap_put(operators, "-", &integerValue);

    integerValue = PLUS;
    HashMap_put(operators, "+", &integerValue);

    integerValue = EQUALS;
    HashMap_put(operators, "=", &integerValue);

    integerValue = OPEN_BRACES;
    HashMap_put(operators, "{", &integerValue);

    integerValue = CLOSE_BRACES;
    HashMap_put(operators, "}", &integerValue);

    integerValue = OPEN_BRACKETS;
    HashMap_put(operators, "[", &integerValue);

    integerValue = CLOSE_BRACKETS;
    HashMap_put(operators, "]", &integerValue);

    integerValue = BITWISE_OR;
    HashMap_put(operators, "|", &integerValue);

    integerValue = SLASH;
    HashMap_put(operators, "/", &integerValue);

    integerValue = QUESTION_MARK;
    HashMap_put(operators, "?", &integerValue);

    integerValue = DOT;
    HashMap_put(operators, ".", &integerValue);

    integerValue = COMMA;
    HashMap_put(operators, ",", &integerValue);

    integerValue = LESS_THAN;
    HashMap_put(operators, "<", &integerValue);

    integerValue = GREATER_THAN;
    HashMap_put(operators, ">", &integerValue);

    integerValue = POSTFIX_INCREMENT;
    HashMap_put(operators, "post++", &integerValue);

    integerValue = POSTFIX_DECREMENT;
    HashMap_put(operators, "post--", &integerValue);

    integerValue = PREFIX_INCREMENT;
    HashMap_put(operators, "pre++", &integerValue);

    integerValue = PREFIX_DECREMENT;
    HashMap_put(operators, "pre--", &integerValue);

    integerValue = CALL;
    HashMap_put(operators, "call", &integerValue);

    integerValue = ACCESS;
    HashMap_put(operators, "access", &integerValue);

    integerValue = ARROW;
    HashMap_put(operators, "->", &integerValue);

    integerValue = CAST;
    HashMap_put(operators, "cast", &integerValue);

    integerValue = UNARY_PLUS;
    HashMap_put(operators, "plus", &integerValue);

    integerValue = UNARY_MINUS;
    HashMap_put(operators, "minus", &integerValue);

    integerValue = LEFT_SHIFT;
    HashMap_put(operators, "<<", &integerValue);

    integerValue = RIGHT_SHIFT;
    HashMap_put(operators, ">>", &integerValue);

    integerValue = LESS_THAN_OR_EQUAL_TO;
    HashMap_put(operators, "<=", &integerValue);

    integerValue = GREATER_THAN_OR_EQUAL_TO;
    HashMap_put(operators, ">=", &integerValue);

    integerValue = EQUAL_EQUAL;
    HashMap_put(operators, "==", &integerValue);

    integerValue = NOT_EQUAL;
    HashMap_put(operators, "!=", &integerValue);

    integerValue = PLUS_EQUALS;
    HashMap_put(operators, "+=", &integerValue);

    integerValue = MINUS_EQUALS;
    HashMap_put(operators, "-=", &integerValue);

    integerValue = TIMES_EQUALS;
    HashMap_put(operators, "*=", &integerValue);

    integerValue = SLASH_EQUALS;
    HashMap_put(operators, "/=", &integerValue);

    integerValue = MOD_EQUALS;
    HashMap_put(operators, "%=", &integerValue);

    integerValue = LEFT_SHIFT_EQUALS;
    HashMap_put(operators, "<<=", &integerValue);

    integerValue = RIGHT_SHIFT_EQUALS;
    HashMap_put(operators, ">>=", &integerValue);

    integerValue = BITWISE_OR_EQUALS;
    HashMap_put(operators, "|=", &integerValue);

    integerValue = BITWISE_AND_EQUALS;
    HashMap_put(operators, "&=", &integerValue);

    integerValue = XOR_EQUALS;
    HashMap_put(operators, "^=", &integerValue);

    integerValue = LOGICAL_OR_EQUALS;
    HashMap_put(operators, "||=", &integerValue);

    integerValue = LOGICAL_AND_EQUALS;
    HashMap_put(operators, "&&=", &integerValue);

    integerValue = SIZEOF;
    HashMap_put(operators, "sizeof", &integerValue);

}

// TODO
void initOperatorPrecedences(HashMap* operatorPrecedences){ }

void initDelimiters(HashMap* delimiters){
    int integerValue = 0;

    integerValue = SEMICOLON;
    HashMap_put(delimiters, ";", &integerValue);

    integerValue = COLON;
    HashMap_put(delimiters, ":", &integerValue);

    integerValue = SPACE;
    HashMap_put(delimiters, " ", &integerValue);

    integerValue = NEWLINE;
    HashMap_put(delimiters, "\n", &integerValue);

    integerValue = TAB;
    HashMap_put(delimiters, "\t", &integerValue);

    integerValue = BACKSLASH;
    HashMap_put(delimiters, "\\", &integerValue);

    integerValue = APOSTROPHE;
    HashMap_put(delimiters, "'", &integerValue);

    integerValue = QUOTE;
    HashMap_put(delimiters, "\"", &integerValue);
}


int stringCompareFunction(void* first, void* second){
    char* firstString = (char*) first;
    char* secondString = (char*) second;
    return !strcmp(firstString, secondString);
}

// initialize the builtin HashMaps
void initBuiltins(){
    dbg("Initializing Builtins...\n");

    // allocate memory for the maps
    BUILTIN_FUNCTIONS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    OPERATORS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    OPERATOR_PRECEDENCES = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    DELIMITERS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);

    // handle the builtin functions
    initBuiltinFunctions(BUILTIN_FUNCTIONS);

    // handle the operators
    initOperators(OPERATORS);

    // handle the operator precedences
    initOperatorPrecedences(OPERATOR_PRECEDENCES);

    // handle delimiters
    initDelimiters(DELIMITERS);
    
    dbg("Finished Initializing builtins\n");
}

// uninitialize the builtin HashMaps
void freeBuiltins(){
    HashMap_free(BUILTIN_FUNCTIONS);
    HashMap_free(OPERATORS);
    HashMap_free(OPERATOR_PRECEDENCES);
    HashMap_free(DELIMITERS);
}


