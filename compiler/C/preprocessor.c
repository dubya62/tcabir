/**
 * The purpose of this file is to handle and remove compiler directives
 */

#include <string.h>

#include "ArrayList.h"
#include "HashMap.h"

#include "preprocessor.h"
#include "token.h"
#include "debug.h"


// TODO
ArrayList* handleConditionalCompilation(ArrayList* tokens){
    return tokens;
}

// TODO
ArrayList* handleIncludes(ArrayList* tokens, HashMap* includedAlready){
    // keep track of what was already included to handle circular includes
    HashMap* childIncludedAlready = HashMap_copy(includedAlready);
    
    int tokensLength = ArrayList_length(tokens);
    if (tokensLength == 1){
        fatal_error(((Token*) ArrayList_get(tokens, 0)), "Expected a filename after include\n");
    }

    // remove the include token
    ArrayList_remove(tokens, 0);

    printf("INCLUDING: %s\n", ArrayList_toString(tokens, Token_toString));

    return tokens;
}

// TODO
ArrayList* handleDefines(ArrayList* tokens){
    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){

    }

    return tokens;
}


ArrayList* handleCompilerDirectives(ArrayList* tokens, ArrayList* includeDirs, HashMap* includedAlready){
    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    Token* hashtag = stringToToken("#");

    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        // the current token is hashtag.
        // next tokens should be the directive
        if (Token_equalsToken(currentToken, hashtag)){
            if (i + 1 >= tokensLength){
                fatal_error(currentToken, "Expected a compiler directive after #.");
            }

            Token* theDirective = (Token*) ArrayList_get(tokens, i+1);
            if (!strcmp(theDirective->token, "define")){
            } else if (!strcmp(theDirective->token, "undef")){
            } else if (!strcmp(theDirective->token, "include")){
                // skip adding tokens until ;
                ArrayList* includedFile = ArrayList_malloc(sizeof(Token));
                i += 1;
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    ArrayList_append(includedFile, (Token*) ArrayList_get(tokens, i));
                    i++;
                }
                includedFile = handleIncludes(includedFile, includedAlready);
                int includedFileLength = ArrayList_length(includedFile);
                for (int j=0; j<includedFileLength; j++){
                    ArrayList_append(result, (Token*) ArrayList_get(includedFile, j));
                }
            } else if (!strcmp(theDirective->token, "pragma")){
            } else if (!strcmp(theDirective->token, "ifndef")){
            } else if (!strcmp(theDirective->token, "ifdef")){
            } else if (!strcmp(theDirective->token, "if")){
            } else if (!strcmp(theDirective->token, "else")){
            } else if (!strcmp(theDirective->token, "error")){
            } else if (!strcmp(theDirective->token, "__FILE__")){
            } else if (!strcmp(theDirective->token, "__LINE__")){
            } else if (!strcmp(theDirective->token, "__DATE__")){
            } else if (!strcmp(theDirective->token, "__TIME__")){
            } else if (!strcmp(theDirective->token, "__TIMESTAMP__")){
            } else {
            }

        }

        ArrayList_append(result, currentToken);
    }


    return result;
}
