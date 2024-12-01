
#include "token.h"
#include "string.h"

// return 1 if the token is equivalent to a given string; 0 otherwise
int Token_equalsString(Token* first, char* theString){
    return strcmp(first->token, theString) == 0;
}

// return 1 if the token is equivalent to another token; 0 otherwise
int Token_equalsToken(Token* first, Token* second){
    return strcmp(first->token, second->token) == 0;
}

char* Token_toString(void* theToken){
    return ((Token*) theToken)->token;
}

