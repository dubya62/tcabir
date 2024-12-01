
#include <stdlib.h>

#include "normalizer.h"
#include "ArrayList.h"
#include "token.h"
#include "debug.h"


// Basic normalization includes the following:
//      removing comments
//      combining strings into single tokens
//      removing tabs, newlines, and spaces
//      combining floats into single tokens


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
                quotes = 0;
            }
        } else if (Token_equalsString(currentToken, "'")){
            if (!quotes && !charQuote){
                charQuote = 1;
            } else if (!quotes && charQuote){
                charQuote = 0;
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
                quotes = 0;
                ArrayList_append(currentString, currentToken);

                Token* newToken = (Token*) malloc(sizeof(Token));
                newToken->filename = currentToken->filename;
                newToken->lineNumber = currentToken->lineNumber;
                newToken->token = ArrayList_toOnlyString(currentString, Token_toString);

                ArrayList_append(result, newToken);
                ArrayList_empty(currentString);
                continue;
            }
        } else if (Token_equalsString(currentToken, "'")){
            if (!charQuotes && !quotes){
                charQuotes = 1;
            } else if (charQuotes && !quotes){
                charQuotes = 0;
                ArrayList_append(currentString, currentToken);

                Token* newToken = (Token*) malloc(sizeof(Token));
                newToken->filename = currentToken->filename;
                newToken->lineNumber = currentToken->lineNumber;
                newToken->token = ArrayList_toOnlyString(currentString, Token_toString);

                ArrayList_append(result, newToken);
                ArrayList_empty(currentString);
                continue;
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










ArrayList* performBasicNormalization(ArrayList* tokens){

    // remove comments and multi-line comments that are not in strings
    tokens = removeComments(tokens);

    // combine strings into single tokens and remove whitespace
    tokens = combineStrings(tokens);

    // combine floats into single tokens
    // TODO: implement this function
    tokens = combineFloats(tokens);


    dbg("Normalizer output:\n");
    dbg("==============================\n");
    dbg(ArrayList_toString(tokens, Token_toString));
    dbg("\n==============================\n");
    return tokens;
}


