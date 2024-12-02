
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

    HashMap_put(types, "const", &currentValue);
    currentValue++;

    HashMap_put(types, "[", &currentValue);
    currentValue++;

    HashMap_put(types, "*", &currentValue);
    currentValue++;

    HashMap_put(types, "]", &currentValue);
    currentValue++;

    HashMap_put(types, "&", &currentValue);
    currentValue++;
}



ArrayList* convertTypeTokens(ArrayList* tokens){
    dbg("Removing Types...\n");

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

            // while still keeping the types of the arguments/members
            Token* typeToken = (Token*) malloc(sizeof(Token));
            typeToken->filename = currentToken->filename;
            typeToken->lineNumber = currentToken->lineNumber;
            typeToken->token = "$TYPE";
            typeToken->type = TYPE;
            int typeTokenIndex = ArrayList_length(result);
            ArrayList_append(result, typeToken);
            while (i < tokensLength){
                Token* ithToken = (Token*) ArrayList_get(tokens, i);

                int isType = HashMap_containsKey(TYPES, ithToken->token);
                // not a type in the case this is &, *, [, ], or comma with nothing before
                if (ithToken->type == OPERATOR && ArrayList_length(currentType) == 0){
                    isType = 0;
                }
                if (isType){
                    ithToken->type = TYPE;
                    ithToken->value = *((int*) HashMap_get(TYPES, ithToken->token));
                    ArrayList_append(currentType, ithToken);
                    i++;
                }
                if (ithToken->type == LITERAL){
                    if (ArrayList_length(currentType) > 0){
                        ArrayList_append(currentType, ithToken);
                        i++;
                        continue;
                    }
                }

                // if this token is a variable, continue checking after it
                if (ithToken->type == VARIABLE){
                    ArrayList_append(result, ithToken);
                    i++;
                    continue;
                }

                if (!isType){
                    ArrayList_append(result, ithToken);
                    break;
                }

            }

            // add this type to the map
            currentToken->token = ArrayList_toOnlyString(currentType, Token_toString);

            // update the value of the $TYPE token and add the type to the HashMap if needed
            Token* changeTypeTokenValue = (Token*) ArrayList_get(result, typeTokenIndex);
            if (HashMap_containsKey(TYPES, currentToken->token)){
                changeTypeTokenValue->value = *((int*) HashMap_get(TYPES, currentToken->token));
            } else {
                int nextTypeNumber = HashMap_size(TYPES);
                HashMap_put(TYPES, currentToken->token, &nextTypeNumber);
                changeTypeTokenValue->value = nextTypeNumber;
            }
            continue;
        }

        ArrayList_append(result, currentToken);
    }

    dbg("Finished Removing Types!\n");
    dbg("With Types Removed:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n");
    dbg(HashMap_toString(TYPES, NULL, NULL));
    dbg("\n==============================\n");

    return result;
}


ArrayList* createExtraScopes(ArrayList* tokens){
    dbg("Creating Extra Scopes...\n");

    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    int tokensLength = ArrayList_length(tokens);
    int openBraces = 0;
    int openedFor = 0;

    Token* openBrace = stringToToken("{"); 
    Token* closeBrace = stringToToken("}"); 
    Token* forToken = stringToToken("for"); 

    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        if (Token_equalsToken(currentToken, forToken)){
            openBrace->filename = currentToken->filename;
            openBrace->lineNumber = currentToken->lineNumber;

            ArrayList_append(result, openBrace);
            openBrace = stringToToken("{");
            // look for the end of this for

            openBraces = 0;
            int j = i + 1;
            while (j < tokensLength){
                Token* jthToken = (Token*) ArrayList_get(tokens, j);
                if (Token_equalsToken(jthToken, openBrace)){
                    openBraces++;
                } else if (Token_equalsToken(jthToken, closeBrace)){
                    openBraces--;
                    if (!openBraces){
                        Token* newToken = stringToToken("}");
                        newToken->filename = jthToken->filename;
                        newToken->lineNumber = jthToken->lineNumber;

                        ArrayList_insert(tokens, j, newToken);
                        tokensLength++;
                        break;
                    }
                }

                j++;
            }
        }
        ArrayList_append(result, currentToken);
    }

    

    // TODO: add extra function scopes

    dbg("Finished Creating Extra Scopes!\n");
    dbg("With Extra Scopes:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n==============================\n");

    return result;
}









ArrayList* performVariableSimplification(ArrayList* tokens){

    // add builtin types to the TYPES HashMap
    addBuiltinTypes(TYPES);

    // convert tokens in the representation that are these types to the type
    tokens = convertTypeTokens(tokens);

    // TODO: create extra scopes for the sake of variable scoping
    tokens = createExtraScopes(tokens);
    
    // TODO: generalize all variables into numbered variables
    
    // TODO: fix the function scoping


    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);
        printf("%s: %d %d\n", currentToken->token, currentToken->type, currentToken->value);
    }



    return tokens;
}


