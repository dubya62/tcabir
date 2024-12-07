/**
 * The purpose of this file is to handle and remove compiler directives
 */

#include <stdio.h>
#include <string.h>

#include "include/ArrayList.h"
#include "include/HashMap.h"

#include "include/preprocessor.h"
#include "include/token.h"
#include "include/debug.h"

#include "include/lexer.h"
#include "include/normalizer.h"


// TODO
ArrayList* handleConditionalCompilation(ArrayList* tokens){
    return tokens;
}


ArrayList* includeFile(char* filename, ArrayList* includeDirs, HashMap* includedAlready){
    // we have a file pointer 
    dbg("PERFORMING PREVIOUS COMPILATION STEPS ON INCLUDED CODE...\n");

    ArrayList* tokens = performLexicalAnalysis(filename);
    tokens = performBasicNormalization(tokens);
    tokens = handleCompilerDirectives(tokens, includeDirs, includedAlready);

    dbg("FINISHED PREVIOUS COMPILATIONS STEPS ON INCLUDED CODE!\n");
    dbg("INCLUDED CODE OUTPUT:\n");
    dbg(ArrayList_toString(tokens, Token_toString));
    dbg("\n");

    return tokens;
}


// TODO
ArrayList* handleIncludes(ArrayList* tokens, ArrayList* includeDirs, HashMap* includedAlready){
    // keep track of what was already included to handle circular includes
    HashMap* childIncludedAlready = HashMap_copy(includedAlready);
    
    int tokensLength = ArrayList_length(tokens);
    if (tokensLength == 1){
        fatal_error(((Token*) ArrayList_get(tokens, 0)), "Expected a filename after include\n");
    }

    // remove the include token
    ArrayList_remove(tokens, 0);
    tokensLength--;

    printf("INCLUDING: %s\n", ArrayList_toString(tokens, Token_toString));

    Token* currentToken = (Token*) ArrayList_get(tokens, 0);
    if (!strcmp(currentToken->token, "<")){
        // this is a standard lib
        
    } else {
        char* filename;
        int filenameLength = strlen(currentToken->token);
        if (filenameLength < 3){
            fatal_error(currentToken, "Expected a filename after include\n");
        }
        filename = currentToken->token + 1;
        currentToken->token[filenameLength-1] = '\0';


        FILE* fp;
        // check current directory for this file
        fp = fopen(filename, "r");
        if (fp != NULL){
            if (HashMap_containsKey(includedAlready, &filename)){
                fatal_error(currentToken, "This include causes a circular import.\n");
            }
            HashMap_put(childIncludedAlready, &filename, "0\0");
            fclose(fp);
            return includeFile(filename, includeDirs, childIncludedAlready);
        }

        // check each directory/header in the includes
        int includeDirsLength = ArrayList_length(includeDirs);
        for (int j=0; j<includeDirsLength; j++){
            char filepath[FILENAME_MAX];
            char* filepathPointer = (char*) filepath;
            char* currentIncludeDir = *((char**)ArrayList_get(includeDirs, j));
            int currentIncludeDirLength = strlen(currentIncludeDir);
            if (currentIncludeDir[currentIncludeDirLength] == '/'){
                currentIncludeDir[currentIncludeDirLength] = '\0';
            }
            snprintf(filepath, FILENAME_MAX, "%s/%s", currentIncludeDir, filename);
            printf("Trying to open %s\n", filepath);
            fp = fopen(filepath, "r");
            if (fp != NULL){
                printf("File successfully found!\n");
                if (HashMap_containsKey(includedAlready, &filepathPointer)){
                    fatal_error(currentToken, "This include causes a circular import.\n");
                }
                HashMap_put(childIncludedAlready, &filepathPointer, "0\0");
                fclose(fp);
                return includeFile(filepath, includeDirs,childIncludedAlready);
            }
            printf("File did not exist...\n");
        }

    }


    return ArrayList_malloc(tokens->memberSize);
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
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "undef")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
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
                includedFile = handleIncludes(includedFile, includeDirs, includedAlready);
                int includedFileLength = ArrayList_length(includedFile);
                for (int j=0; j<includedFileLength; j++){
                    ArrayList_append(result, (Token*) ArrayList_get(includedFile, j));
                }
                continue;
            } else if (!strcmp(theDirective->token, "pragma")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "ifndef")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "ifdef")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "if")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "else")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "error")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "__FILE__")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "__LINE__")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "__DATE__")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "__TIME__")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else if (!strcmp(theDirective->token, "__TIMESTAMP__")){
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            } else {
                // TODO
                while (i < tokensLength){
                    if (!strcmp(((Token*)ArrayList_get(tokens, i))->token, ";")){
                        break;
                    }
                    i++;
                }
                continue;
            }

        }

        ArrayList_append(result, currentToken);
    }


    return result;
}
