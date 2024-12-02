#ifndef BUILTINS_H
#define BUILTINS_H

#include "HashMap.h"
#include "token.h"

extern HashMap* BUILTIN_FUNCTIONS;
extern HashMap* OPERATORS;
extern HashMap* OPERATOR_PRECEDENCES;
extern HashMap* DELIMITERS;


// initialize the builtin HashMaps
void initBuiltins();
// uninitialize the builtin HashMaps
void freeBuiltins();


#endif
