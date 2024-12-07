
#include <stdlib.h>
#include <string.h>

#include "include/ArrayList.h"

#include "include/normalizer.h"
#include "include/token.h"
#include "include/debug.h"


// Basic normalization includes the following:
//      removing comments
//      combining strings into single tokens
//      removing tabs, newlines, and spaces
//      combining floats into single tokens


// return number of backslashes directly before a token
int getBackslashesBefore(ArrayList* tokens, int index){
    int backslashes = 0;
    int j = index-1;
    while (j > 0){
        if (Token_equalsString(ArrayList_get(tokens, j), "\\")){
            backslashes++;
        } else {
            break;
        }
        j--;
    }
    return backslashes;
}


ArrayList* removeComments(ArrayList* tokens){
    dbg("Removing comments from the tokens...\n");

    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    int comment = 0;
    int multiLine = 0;
    int quotes = 0;
    int charQuote = 0;

    int numberOfTokens = ArrayList_length(tokens);
    Token* currentToken;
    Token* lastToken;
    Token* nextToken;
    for (int i=0; i<numberOfTokens; i++){

        currentToken = ArrayList_get(tokens, i);


        if (Token_equalsString(currentToken, "/")){
            if (comment && multiLine){
                if (i && Token_equalsString(ArrayList_get(tokens, i-1), "*")){
                    comment = 0;
                    multiLine = 0;
                    continue;
                }
            }

            if (i + 1 < numberOfTokens){
                nextToken = ArrayList_get(tokens, i+1);
                if (Token_equalsString(nextToken, "/")){
                    if (!comment && !quotes){
                        comment = 1;
                        multiLine = 0;
                    }
                } else if (Token_equalsString(nextToken, "*")){
                    if (!comment && !quotes){
                        comment = 1;
                        multiLine = 1;
                    }
                }
            }
        }

        // keeping track of whether or not we are in quotes
        else if (Token_equalsString(currentToken, "\"")){
            if (!quotes && !charQuote){
                quotes = 1;
            } else if (quotes && !charQuote){
                // make sure there are an even number of backslashes before
                int backslashes = getBackslashesBefore(tokens, i);

                if (backslashes % 2 == 0){
                    quotes = 0;
                }
            }
        } else if (Token_equalsString(currentToken, "'")){
            if (!quotes && !charQuote){
                charQuote = 1;
            } else if (!quotes && charQuote){
                // make sure there are an even number of backslashes before
                int backslashes = getBackslashesBefore(tokens, i);

                if (backslashes % 2 == 0){
                    charQuote = 0;
                }
            }
        }



        // keeping track of newlines
        else if (Token_equalsString(currentToken, "\n")){
            // end non multi-line comments
            if (comment && !multiLine){
                comment = 0;
            }
            // throw a syntax error if in a string and the last char is not backslash
            if (quotes && i){
                lastToken = ArrayList_get(tokens, i-1);
                if (!Token_equalsString(lastToken, "\\")){
                    fatal_error(lastToken, "Syntax error. Unmatched \"");
                }
            }
        }

        if (!comment){
            ArrayList_append(result, currentToken);
        }

    }

    ArrayList_free(tokens);

    dbg("Finished removing comments from the tokens!\n");
    dbg("With Comments Removed:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n==============================\n");

    return result;
}



ArrayList* combineStrings(ArrayList* tokens){
    dbg("Combining String and Char literals to single tokens...\n");

    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    int quotes = 0;
    int charQuotes = 0;

    ArrayList* currentString = ArrayList_malloc(sizeof(Token));

    int tokensLength = ArrayList_length(tokens);
    Token* currentToken;
    for (int i=0; i<tokensLength; i++){
        currentToken = ArrayList_get(tokens, i);

        if (Token_equalsString(currentToken, "\"")){
            if (!charQuotes && !quotes){
                quotes = 1;
            } else if (!charQuotes && quotes){
                // make sure there are an even number of backslashes before
                int backslashes = getBackslashesBefore(tokens, i);

                if (backslashes % 2 == 0){
                    quotes = 0;
                    ArrayList_append(currentString, currentToken);

                    Token* newToken = (Token*) malloc(sizeof(Token));
                    newToken->filename = currentToken->filename;
                    newToken->lineNumber = currentToken->lineNumber;
                    newToken->token = ArrayList_toOnlyString(currentString, Token_toString);

                    newToken->type = LITERAL;
                    newToken->value = STRING;

                    ArrayList_append(result, newToken);
                    ArrayList_empty(currentString);
                    continue;
                }
            }
        } else if (Token_equalsString(currentToken, "'")){
            if (!charQuotes && !quotes){
                charQuotes = 1;
            } else if (charQuotes && !quotes){
                // make sure there are an even number of backslashes before
                int backslashes = getBackslashesBefore(tokens, i);

                if (backslashes % 2 == 0){
                    charQuotes = 0;
                    ArrayList_append(currentString, currentToken);

                    Token* newToken = (Token*) malloc(sizeof(Token));
                    newToken->filename = currentToken->filename;
                    newToken->lineNumber = currentToken->lineNumber;
                    newToken->token = ArrayList_toOnlyString(currentString, Token_toString);

                    newToken->type = LITERAL;
                    newToken->value = CHAR;

                    ArrayList_append(result, newToken);
                    ArrayList_empty(currentString);
                    continue;
                }
            }
        } else if (Token_equalsString(currentToken, "\\")){
            if (i+1 < tokensLength){
                if (Token_equalsString(ArrayList_get(tokens, i+1), "\n")){
                    i++;
                    continue;
                }
            }
        }
        
        if (quotes || charQuotes){
            ArrayList_append(currentString, currentToken);
        } else {
            // remove whitespace
            if (!Token_equalsString(currentToken, " ") && !Token_equalsString(currentToken, "\n") && !Token_equalsString(currentToken, "\t")){
                ArrayList_append(result, currentToken);
            }
        }

    }


    ArrayList_free(currentString);
    ArrayList_free(tokens);

    dbg("Finished Combining String and Char literals!\n");
    dbg("With Literals Combined (and whitespace removed):\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n==============================\n");
    
    return result;
}


ArrayList* combineFloats(ArrayList* tokens){
    dbg("Combining Floats into single tokens...\n");
    ArrayList* result = ArrayList_malloc(tokens->memberSize);

    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);
        if (currentToken->type == LITERAL && currentToken->value == INT){
            if (i + 2 < tokensLength){
                // next token should be a dot and the next be another integer literal
                Token* nextToken = (Token*) ArrayList_get(tokens, i+1);
                Token* theNextToken = (Token*) ArrayList_get(tokens, i+2);
                if (nextToken->type == OPERATOR && nextToken->value == DOT){
                    if (theNextToken->type == LITERAL && theNextToken->value == INT){
                        // append a new token that is these three tokens concatenated
                        int bufferLen = strlen(currentToken->token) + strlen(nextToken->token) + strlen(theNextToken->token) + 1;
                        char* newTokenValue = (char*) malloc(sizeof(char) * bufferLen);
                        newTokenValue[0] = '\0';

                        strcat(newTokenValue, currentToken->token);
                        strcat(newTokenValue, nextToken->token);
                        strcat(newTokenValue, theNextToken->token);

                        free(currentToken->token);
                        currentToken->token = newTokenValue;

                        // update the state information
                        currentToken->type = LITERAL;
                        currentToken->value = FLOAT;

                        ArrayList_append(result, currentToken);
                        // skip the next two tokens since we combined
                        i++;
                        i++;
                        continue;
                    }
                }
            }
        }
        ArrayList_append(result, currentToken);
    }

    ArrayList_free(tokens);

    dbg("Finished Combining Floats Into Single Tokens!\n");
    dbg("With Floats Combined:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(result, Token_toString));
    dbg("\n==============================\n");

    return result;
}

ArrayList* checkDirectiveSyntax(ArrayList* tokens){
    // make sure there is a newline at the end of compiler directives.
    // replace the newline with a semicolon
    int tokensLength = ArrayList_length(tokens);
    Token* newToken;
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);
        if (Token_equalsString(currentToken, "#")){
            // this is a compiler directive
            // if ; is reached before newline, throw error
            int fixed = 0;
            Token* ithToken;
            while (i < tokensLength){
                ithToken = (Token*) ArrayList_get(tokens, i);
                if (Token_equalsString(ithToken, ";")){
                    error(ithToken, "Preprocessor Directives should end with a newline.");
                    fixed = 1;
                    break;
                } else if (Token_equalsString(ithToken, "\n")){
                    newToken = stringToToken(";");
                    newToken->filename = ithToken->filename;
                    newToken->lineNumber = ithToken->lineNumber;
                    *((Token*) ArrayList_get(tokens, i)) = *newToken;
                    fixed = 1;
                    break;
                }
                i++;
            }
            if (!fixed){
                newToken = stringToToken(";");
                newToken->filename = ithToken->filename;
                newToken->lineNumber = ithToken->lineNumber;
                ArrayList_append(tokens, newToken);
            }

        }
    }

    return tokens;
}


ArrayList* performBasicNormalization(ArrayList* tokens){

    // remove comments and multi-line comments that are not in strings
    tokens = removeComments(tokens);

    // check compiler directive syntax
    tokens = checkDirectiveSyntax(tokens);

    // combine strings into single tokens and remove whitespace
    tokens = combineStrings(tokens);

    // combine floats into single tokens
    tokens = combineFloats(tokens);


    dbg("Normalizer output:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(tokens, Token_toString));
    dbg("\n==============================\n");
    return tokens;
}


