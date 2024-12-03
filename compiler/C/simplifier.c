
#include "string.h"

#include "ArrayList.h"
#include "HashMap.h"

#include "debug.h"
#include "token.h"
#include "builtins.h"
#include "simplifier.h"


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
        if (HashMap_containsKey(TYPES, &currentToken->token)){
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

                int isType = HashMap_containsKey(TYPES, &ithToken->token);
                // not a type in the case this is &, *, [, ], or comma with nothing before
                if (ithToken->type == OPERATOR && ArrayList_length(currentType) == 0){
                    isType = 0;
                }
                if (isType){
                    ithToken->type = TYPE;
                    ithToken->value = *((int*) HashMap_get(TYPES, &ithToken->token));
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
            if (HashMap_containsKey(TYPES, &currentToken->token)){
                changeTypeTokenValue->value = *((int*) HashMap_get(TYPES, &currentToken->token));
            } else {
                int nextTypeNumber = HashMap_size(TYPES);
                HashMap_put(TYPES, &currentToken->token, &nextTypeNumber);
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


ArrayList* createExtraFunctionScopes(ArrayList* tokens){
    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    Token* openParen = stringToToken("(");
    Token* closeParen = stringToToken(")");
    Token* openBrace = stringToToken("{");
    Token* closeBrace = stringToToken("}");
    Token* semicolon = stringToToken(";");

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


