
#include <stdlib.h>
#include <string.h>

#include "include/HashMap.h"

#include "include/debug.h"
#include "include/builtins.h"
#include "include/structure.h"


char* returnSelf(void* str){
    return (char*) str;
}

char* stringMalloc(char* theString){
    int stringLen = strlen(theString);
    char* result = (char*) malloc(sizeof(char) * (stringLen + 1));
    strcpy(result, theString);
    result[stringLen] = '\0';
    return result;
}

// handle BUILTIN_FUNCTIONS
void initBuiltinFunctions(HashMap* builtinFunctions){
    int integerValue = 0;

    char* stringValue;

    integerValue = MALLOC;
    stringValue = stringMalloc("malloc");
    HashMap_put(builtinFunctions, &stringValue, &integerValue);

    integerValue = FREE;
    stringValue = stringMalloc("free");
    HashMap_put(builtinFunctions, &stringValue, &integerValue);

    integerValue = READ;
    stringValue = stringMalloc("read");
    HashMap_put(builtinFunctions, &stringValue, &integerValue);

    integerValue = WRITE;
    stringValue = stringMalloc("write");
    HashMap_put(builtinFunctions,&stringValue, &integerValue);

}

// handle OPERATORS
void initOperators(HashMap* operators){
    int integerValue = 0;
    char* stringValue;

    integerValue = BITWISE_NOT;
    stringValue = stringMalloc("~");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LOGICAL_NOT;
    stringValue = stringMalloc("!");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = HASHTAG;
    stringValue = stringMalloc("#");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = MOD;
    stringValue = stringMalloc("%");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = XOR;
    stringValue = stringMalloc("^");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = BITWISE_AND;
    stringValue = stringMalloc("&");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = TIMES;
    stringValue = stringMalloc("*");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = REFERENCE;
    stringValue = stringMalloc("ref");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = DEREFERENCE;
    stringValue = stringMalloc("deref");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = OPEN_PARENTHESIS;
    stringValue = stringMalloc("(");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = CLOSE_PARENTHESIS;
    stringValue = stringMalloc(")");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = MINUS;
    stringValue = stringMalloc("-");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = PLUS;
    stringValue = stringMalloc("+");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = EQUALS;
    stringValue = stringMalloc("=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = OPEN_BRACES;
    stringValue = stringMalloc("{");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = CLOSE_BRACES;
    stringValue = stringMalloc("}");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = OPEN_BRACKETS;
    stringValue = stringMalloc("[");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = CLOSE_BRACKETS;
    stringValue = stringMalloc("]");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = BITWISE_OR;
    stringValue = stringMalloc("|");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = SLASH;
    stringValue = stringMalloc("/");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = QUESTION_MARK;
    stringValue = stringMalloc("?");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = DOT;
    stringValue = stringMalloc(".");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = COMMA;
    stringValue = stringMalloc(",");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LESS_THAN;
    stringValue = stringMalloc("<");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = GREATER_THAN;
    stringValue = stringMalloc(">");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = POSTFIX_INCREMENT;
    stringValue = stringMalloc("post++");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = POSTFIX_DECREMENT;
    stringValue = stringMalloc("post--");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = PREFIX_INCREMENT;
    stringValue = stringMalloc("pre++");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = PREFIX_DECREMENT;
    stringValue = stringMalloc("pre--");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = CALL;
    stringValue = stringMalloc("call");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = ACCESS;
    stringValue = stringMalloc("access");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = ARROW;
    stringValue = stringMalloc("->");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = CAST;
    stringValue = stringMalloc("cast");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = UNARY_PLUS;
    stringValue = stringMalloc("plus");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = UNARY_MINUS;
    stringValue = stringMalloc("minus");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LEFT_SHIFT;
    stringValue = stringMalloc("<<");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = RIGHT_SHIFT;
    stringValue = stringMalloc(">>");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LESS_THAN_OR_EQUAL_TO;
    stringValue = stringMalloc("<=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = GREATER_THAN_OR_EQUAL_TO;
    stringValue = stringMalloc(">=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = EQUAL_EQUAL;
    stringValue = stringMalloc("==");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = NOT_EQUAL;
    stringValue = stringMalloc("!=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = PLUS_EQUALS;
    stringValue = stringMalloc("+=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = MINUS_EQUALS;
    stringValue = stringMalloc("-=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = TIMES_EQUALS;
    stringValue = stringMalloc("*=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = SLASH_EQUALS;
    stringValue = stringMalloc("/=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = MOD_EQUALS;
    stringValue = stringMalloc("%=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LEFT_SHIFT_EQUALS;
    stringValue = stringMalloc("<<=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = RIGHT_SHIFT_EQUALS;
    stringValue = stringMalloc(">>=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = BITWISE_OR_EQUALS;
    stringValue = stringMalloc("|=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = BITWISE_AND_EQUALS;
    stringValue = stringMalloc("&=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = XOR_EQUALS;
    stringValue = stringMalloc("^=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LOGICAL_OR_EQUALS;
    stringValue = stringMalloc("||=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = LOGICAL_AND_EQUALS;
    stringValue = stringMalloc("&&=");
    HashMap_put(operators, &stringValue, &integerValue);

    integerValue = SIZEOF;
    stringValue = stringMalloc("sizeof");
    HashMap_put(operators, &stringValue, &integerValue);

}

// TODO
void initOperatorPrecedences(HashMap* operatorPrecedences){ }

void initDelimiters(HashMap* delimiters){
    int integerValue = 0;
    char* stringValue;

    integerValue = SEMICOLON;
    stringValue = stringMalloc(";");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = COLON;
    stringValue = stringMalloc(":");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = SPACE;
    stringValue = stringMalloc(" ");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = NEWLINE;
    stringValue = stringMalloc("\n");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = TAB;
    stringValue = stringMalloc("\t");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = BACKSLASH;
    stringValue = stringMalloc("\\");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = APOSTROPHE;
    stringValue = stringMalloc("'");
    HashMap_put(delimiters, &stringValue, &integerValue);

    integerValue = QUOTE;
    stringValue = stringMalloc("\"");
    HashMap_put(delimiters, &stringValue, &integerValue);
}


void initStatements(HashMap* statements){
    int integerValue = 0;
    char* stringValue;

    integerValue = RETURN;
    stringValue = stringMalloc("return");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = BREAK;
    stringValue = stringMalloc("break");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = IF;
    stringValue = stringMalloc("if");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = ELSE;
    stringValue = stringMalloc("else");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = FOR;
    stringValue = stringMalloc("for");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = WHILE;
    stringValue = stringMalloc("while");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = SWITCH;
    stringValue = stringMalloc("switch");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = CONTINUE;
    stringValue = stringMalloc("continue");
    HashMap_put(statements, &stringValue, &integerValue);

    integerValue = TYPEDEF;
    stringValue = stringMalloc("typedef");
    HashMap_put(statements, &stringValue, &integerValue);

}



// initialize the builtin HashMaps
void initBuiltins(){
    dbg("Initializing Builtins...\n");

    // allocate memory for the maps
    BUILTIN_FUNCTIONS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    OPERATORS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    OPERATOR_PRECEDENCES = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    DELIMITERS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    TYPES = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    VARIABLE_TYPES = HashMap_malloc(sizeof(int), sizeof(int), &stringPrehashFunction, NULL);
    STATEMENTS = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    VARIABLE_NAMES = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    STRUCTURES = HashMap_malloc(sizeof(char*), sizeof(Structure), &stringPrehashFunction, &stringCompareFunction); // name:Structure


    // handle the builtin functions
    initBuiltinFunctions(BUILTIN_FUNCTIONS);

    // handle the operators
    initOperators(OPERATORS);

    // handle the operator precedences
    initOperatorPrecedences(OPERATOR_PRECEDENCES);

    // handle delimiters
    initDelimiters(DELIMITERS);

    // handle statments
    initStatements(STATEMENTS);

    dbg("Finished Initializing builtins\n");
}

// uninitialize the builtin HashMaps
void freeBuiltins(){
    HashMap_free(BUILTIN_FUNCTIONS);
    HashMap_free(OPERATORS);
    HashMap_free(OPERATOR_PRECEDENCES);
    HashMap_free(DELIMITERS);

    HashMap_free(TYPES);
    HashMap_free(VARIABLE_TYPES);
    HashMap_free(STATEMENTS);
    HashMap_free(VARIABLE_NAMES);
    HashMap_free(STRUCTURES);
}


