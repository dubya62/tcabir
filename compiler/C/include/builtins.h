#ifndef BUILTINS_H
#define BUILTINS_H

#include "HashMap.h"
#include "token.h"

extern HashMap* BUILTIN_FUNCTIONS;
extern HashMap* OPERATORS;
extern HashMap* OPERATOR_PRECEDENCES;
extern HashMap* DELIMITERS;

extern HashMap* TYPES; // index the types
extern HashMap* VARIABLE_TYPES; // keep track of the type of each variable (using the TYPES Map (VARIABLE_NUMBER:TYPE_NUMBER)
extern HashMap* STATEMENTS;

// initialize the builtin HashMaps
void initBuiltins();
// uninitialize the builtin HashMaps
void freeBuiltins();


#endif
