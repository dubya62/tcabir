#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#include "token.h"

// Macros for printing debug information
#define DEBUG 1
#define dbg(message) if(DEBUG){printf("%s", message);}
#define panic(message) dbg(message);exit(1);
#define error(token, message) printf("\n------------------------------\nERROR in %s:%d\n\t%s\n", token->filename, token->lineNumber, message);
#define fatal_error(token, message) error(token,message); panic("\nEncountered a fatal error. Exiting...\n\n");

#endif
