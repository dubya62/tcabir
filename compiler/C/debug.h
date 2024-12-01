#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>

// Macros for printing debug information
#define DEBUG 1
#define dbg(message) if(DEBUG){printf(message);}
#define panic(message) dbg(message);exit(1);

#endif
