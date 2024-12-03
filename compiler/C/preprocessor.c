/**
 * The purpose of this file is to handle and remove compiler directives
 */

#include "ArrayList.h"

#include "preprocessor.h"


ArrayList* handleIncludes(ArrayList* tokens){
    return tokens;
}



ArrayList* handleDefines(ArrayList* tokens){
    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){

    }

    return tokens;
}





ArrayList* handleCompilerDirectives(ArrayList* tokens){


    return tokens;
}
