
#include "ArrayList.h"
#include "HashMap.h"

#include "debug.h"
#include "token.h"
#include "builtins.h"
#include "simplifier.h"


void addBuiltinTypes(HashMap* types){
    int currentValue = 0;

    HashMap_put(types, "char", &currentValue);
    currentValue++;

    HashMap_put(types, "int", &currentValue);
    currentValue++;

    HashMap_put(types, "float", &currentValue);
    currentValue++;

    HashMap_put(types, "double", &currentValue);
    currentValue++;

    HashMap_put(types, "short", &currentValue);
    currentValue++;

    HashMap_put(types, "void", &currentValue);
    currentValue++;

    HashMap_put(types, "long", &currentValue);
    currentValue++;

    HashMap_put(types, "signed", &currentValue);
    currentValue++;

    HashMap_put(types, "unsigned", &currentValue);
    currentValue++;

    HashMap_put(types, "struct", &currentValue);
    currentValue++;

    HashMap_put(types, "size_t", &currentValue);
    currentValue++;

    HashMap_put(types, "ssize_t", &currentValue);
    currentValue++;
}


ArrayList* convertTypeTokens(ArrayList* tokens){

    ArrayList* result = ArrayList_malloc(tokens->memberSize);
    ArrayList* currentType = ArrayList_malloc(tokens->memberSize);

    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        // concatenate types into single tokens and give them a place in the types Map
        Token* currentToken = (Token*) ArrayList_get(tokens, i);
        // this is a builtin or a type already encountered
        if (HashMap_containsKey(TYPES, currentToken->token)){
            // combine any tokens after that can be type tokens
            ArrayList_empty(currentType);

            // TODO: try to capture function and struct types
            // while still keeping the types of the arguments/members
            int j=i;
            while (j < tokensLength){
                Token* jthToken = (Token*) ArrayList_get(tokens, j);
                // if this token is ) break
                if (jthToken->type == OPERATOR && jthToken->value == CLOSE_PARENTHESIS){
                    break;
                }
                // if this token is a variable, continue checking after it
                if (jthToken->type == VARIABLE){
                    j++;
                }


                ArrayList_append(currentType, jthToken);
                j++;
            }

            printf("%s\n", ArrayList_toString(currentType, Token_toString));
            
        }
    }


    return result;
}











ArrayList* performVariableSimplification(ArrayList* tokens){

    // add builtin types to the TYPES HashMap
    addBuiltinTypes(TYPES);

    // convert tokens in the representation that are these types to the type
    tokens = convertTypeTokens(tokens);

    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);
        printf("%s: %d %d\n", currentToken->token, currentToken->type, currentToken->value);
    }



    return tokens;
}


