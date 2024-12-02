#ifndef LEXER_H
#define LEXER_H

#include "ArrayList.h"

// Break a file up into tokens and return the tokens
// as an array of string
ArrayList* performLexicalAnalysis(char* filename);

#endif
