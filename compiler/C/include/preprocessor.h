#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "ArrayList.h"

ArrayList* handleCompilerDirectives(ArrayList* tokens, ArrayList* includeDirs, HashMap* includedAlready);

#endif
