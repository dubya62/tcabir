

#include "ArrayList.h"

#include "converter.h"
#include "debug.h"


ArrayList* convertElseIfs(ArrayList* tokens){
    int tokensLength = ArrayList_length(tokens);
    for (int i=0; i<tokensLength; i++){
        Token* currentToken = (Token*) ArrayList_get(tokens, i);

        if (Token_equalsString(currentToken, "else")){
            if (i + 1 >= tokensLength){
                fatal_error(currentToken, "Expected tokens after else.\n");
            }
            Token* nextToken = (Token*) ArrayList_get(tokens, i+1);
            if (Token_equalsString(nextToken, "if")){
                Token* openBrace = stringToToken("{");
                openBrace->lineNumber = currentToken->lineNumber;
                openBrace->filename = currentToken->filename;
                ArrayList_insert(tokens, i+1, openBrace);
                tokensLength++;
                // look for the closing brace to the if and put another closing brace after
                int openBraces = 0;
                int j = i + 2;
                int done = 0;
                while (j < tokensLength){
                    Token* jthToken = (Token*) ArrayList_get(tokens, j);
                    if (jthToken->type == OPERATOR){
                        switch(jthToken->value){
                            case OPEN_BRACES:
                                openBraces++;
                                break;
                            case CLOSE_BRACES:
                                openBraces--;
                                // keep going if next token is else
                                if (openBraces == 0 && !(j + 1 < tokensLength && Token_equalsString((Token*)ArrayList_get(tokens, j+1), "else"))){
                                    done = 1;
                                }
                                break;
                        }
                        if (done){
                            break;
                        }
                    }
                    j++;
                }
                Token* closeBrace = stringToToken("}");
                closeBrace->lineNumber = currentToken->lineNumber;
                closeBrace->filename= currentToken->filename;
                ArrayList_insert(tokens, j, closeBrace);
            }

        }

    }


    return tokens;
}




ArrayList* convertLoops(ArrayList* tokens){
    // convert else if (){} into else {if(){}}
    tokens = convertElseIfs(tokens);

    dbg("Loop Conversion Output:\n");
    dbg(ArrayList_toString(tokens, Token_toString));
    dbg("\n");

    return tokens;
}

