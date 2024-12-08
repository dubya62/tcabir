
#include <string.h>

#include "include/ArrayList.h"
#include "include/HashMap.h"

#include "include/debug.h"
#include "include/token.h"
#include "include/builtins.h"
#include "include/simplifier.h"


void addBuiltinTypes(HashMap* types){
    int currentValue = 0;
    char* stringValue;

    stringValue = stringMalloc("char");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("int");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("float");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("double");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("short");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("void");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("long");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("signed");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("unsigned");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("struct");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("size_t");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("ssize_t");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("const");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("[");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("*");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("]");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("&");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("auto");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("static");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;
    
    stringValue = stringMalloc("register");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("extern");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;
    
    stringValue = stringMalloc("enum");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;

    stringValue = stringMalloc("union");
    HashMap_put(types, &stringValue, &currentValue);
    currentValue++;
}




ArrayList* convertTypeTokens(ArrayList* tokens){
    dbg("Removing Types...\n");

    ArrayList* result = ArrayList_malloc(tokens->memberSize);
    ArrayList* currentType = ArrayList_malloc(tokens->memberSize);

    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        // if this is a struct, handle inside then outside
        if (!strcmp(currentToken->token, "struct\0") || !strcmp(currentToken->token, "union\0")){
            int returnIndex = i;
            int openBraces = 0;

            int j = i + 1;
            // gather tokens inside this struct/union
            while (j < tokensLength) {
                Token* jthToken = (Token*) ArrayList_get(tokens, j);
                if (jthToken->type == OPERATOR){
                    if (jthToken->value == OPEN_BRACES){
                        openBraces++;
                    } else if (jthToken->value == CLOSE_BRACES) {
                        openBraces--;
                    }
                }
                j++;
            }
            // run this function on stuff inside


        } else if (!strcmp(currentToken->token, "enum\0")){
            
        } else {
            // handle normal functions and normal tokens
        }

        ArrayList_append(result, currentToken);
    }

    dbg("Finished Removing Types!\n");
    dbg("With Types Removed:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n");
    dbg(HashMap_toString(TYPES, &stringToStringFunction, NULL));
    dbg("\n==============================\n");

    return result;
}


// handle typedef tokens
ArrayList* handleTypedefs(ArrayList* tokens){
    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    dbg("Handling Typedefs...\n");
    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        if (currentToken->type == STATEMENT && currentToken->value == TYPEDEF){
            // this is a typedef
            // next token should be $TYPE
            // could be a structure, enum, union, 
            if (i + 3 >= tokensLength){
                fatal_error(currentToken, "Expected a type declaration after typdef\n");
            }
            Token* nextToken = (Token*) ArrayList_get(tokens, i+1);
            // next token should be a type
            if (nextToken->type != TYPE){
                fatal_error(nextToken, "Expected an existing type after typedef\n");
            }
            // if the tokens after's type was struct, 

        }

        ArrayList_append(result, currentToken);
    }

    dbg("Finished Handling Typedefs!\n");
    dbg("With Typedefs Handled:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n==============================\n");

    return result;
}



ArrayList* createExtraFunctionScopes(ArrayList* tokens){
    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    Token* openBrace = stringToToken("{");
    Token* closeBrace = stringToToken("}");

    int openParens = 0;
    int openBraces = 0;

    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        // look for a type followed by a variable,
        // followed by (...), followed by { or ;
        if (currentToken->type == TYPE){
            if (i + 2 < tokensLength){
                Token* nextToken = ArrayList_get(tokens, i+1);
                Token* theNextToken = ArrayList_get(tokens, i+2);
                if (nextToken->type == VARIABLE && theNextToken->type == OPERATOR && theNextToken->value == OPEN_PARENTHESIS){
                    int j = i+2;
                    int found = 0;
                    while (j < tokensLength){
                        Token* jthToken = (Token*) ArrayList_get(tokens, j);

                        if (jthToken->type == OPERATOR){
                            switch(jthToken->value){
                                case OPEN_PARENTHESIS:
                                    openParens++;
                                    break;
                                case CLOSE_PARENTHESIS:
                                    openParens--;
                                    if (!openParens){
                                        // the next token needs to be { or ;
                                        if (j + 1 < tokensLength && ArrayList_get(tokens, j+1)){
                                            openBrace->lineNumber = theNextToken->lineNumber;
                                            openBrace->filename = theNextToken->filename;
                                            ArrayList_insert(tokens, i+2, openBrace);
                                            openBrace = stringToToken("{");
                                            // look for the closing brace and place another } there
                                            tokensLength++;
                                            j++;
                                            int done = 0;
                                            while (j < tokensLength){
                                                jthToken = (Token*) ArrayList_get(tokens, j);
                                                if (jthToken->type == OPERATOR){
                                                    switch (jthToken->value){
                                                        case OPEN_BRACES:
                                                            openBraces++;
                                                            break;
                                                        case CLOSE_BRACES:
                                                            openBraces--;
                                                            if (!openBraces){
                                                                closeBrace->filename = jthToken->filename;
                                                                closeBrace->lineNumber = jthToken->lineNumber;
                                                                
                                                                ArrayList_insert(tokens, j, closeBrace);
                                                                tokensLength++;
                                                                closeBrace = stringToToken("}");
                                                                done = 1;
                                                            }
                                                            break;
                                                    }
                                                }
                                                if (done){
                                                    found = 1;
                                                    break;
                                                }
                                                
                                                j++;
                                            }
                                        }
                                    }
                                    break;
                            }
                        }

                        if (found){
                            break;
                        }

                        j++;

                    }
                }
            }
        }

        ArrayList_append(result, currentToken);

    }

    return result;
}


ArrayList* createExtraScopes(ArrayList* tokens){
    dbg("Creating Extra Scopes...\n");

    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    int tokensLength = ArrayList_length(tokens);
    int openBraces = 0;

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

    
    // add extra function scopes
    //result = createExtraFunctionScopes(result);

    dbg("Finished Creating Extra Scopes!\n");
    dbg("With Extra Scopes:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n==============================\n");

    return result;
}



ArrayList* generalizeVariables(ArrayList* tokens){
    int tokensLength = ArrayList_length(tokens);

    ArrayList* scopes = ArrayList_malloc(sizeof(HashMap));

    // [{var1, var2}, {var1, var2}, {var3, var4}]

    HashMap* newHashMap = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
    ArrayList_append(scopes, newHashMap);
    int currentScope = 0;

    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        if (currentToken->type == VARIABLE){
            // search through each scope for the variable
            int j = currentScope;
            int foundScope = -1;
            while (j >= 0){
                if (HashMap_containsKey((HashMap*) ArrayList_get(scopes, j), &currentToken->token)){
                    foundScope = j;
                    break;
                }
                j--;
            }

            if (foundScope != -1){
                if (i > 0 && ((Token*)ArrayList_get(tokens, i-1))->type == TYPE){
                    fatal_error(currentToken, "Multiple declarations of same variable in this scope.\n");
                }
                currentToken->value = *((int*)HashMap_get(VARIABLE_NAMES, &currentToken->token));
            } else {
                // if token before is not $TYPE, it was never declared
                if (i == 0 || ((Token*)ArrayList_get(tokens, i-1))->type != TYPE){
                    printf("%s\n", currentToken->token);
                    fatal_error(currentToken, "Variable was used before it was declared.\n");
                }

                int nextValue = HashMap_size(VARIABLE_NAMES);

                char* variableName = stringMalloc(currentToken->token);
                HashMap_put(VARIABLE_NAMES, &variableName, &nextValue);

                variableName = stringMalloc(currentToken->token);
                HashMap_put((HashMap*) ArrayList_get(scopes, currentScope), &variableName, &nextValue);
                currentToken->value = nextValue;
            }
        } else if (currentToken->type == OPERATOR){
            switch (currentToken->value){
                case OPEN_BRACES:
                    // create a new scope
                    newHashMap = HashMap_malloc(sizeof(char*), sizeof(int), &stringPrehashFunction, &stringCompareFunction);
                    ArrayList_append(scopes, newHashMap);
                    currentScope++;
                    break;
                case CLOSE_BRACES:
                    // destroy last scope
                    HashMap_free((HashMap*) ArrayList_get(scopes, currentScope));
                    ArrayList_remove(scopes, currentScope);
                    currentScope--;
                    if (currentScope < 0){
                        fatal_error(currentToken, "Mismatched Braces...");
                    }
                    break;
            }
        }

    }
    return tokens;
}



ArrayList* performVariableSimplification(ArrayList* tokens){

    // add builtin types to the TYPES HashMap
    addBuiltinTypes(TYPES);

    // convert tokens in the representation that are these types to the type
    tokens = convertTypeTokens(tokens);

    // handle typedefs
    tokens = handleTypedefs(tokens);

    // create extra scopes for the sake of variable scoping
    tokens = createExtraScopes(tokens);
    
    // generalize all variables into numbered variables
    tokens = generalizeVariables(tokens);
    
    // TODO: fix the function scoping


    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);
        printf("%s: %d %d\n", currentToken->token, currentToken->type, currentToken->value);
    }



    return tokens;
}


