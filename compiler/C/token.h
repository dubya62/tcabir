#ifndef TOKEN_H
#define TOKEN_H

typedef struct Token {
    int lineNumber;
    char* filename;
    char* token;
} Token;

// return 1 if the token is equivalent to a given string; 0 otherwise
int Token_equalsString(Token* first, char* theString);
// return 1 if the token is equivalent to another token; 0 otherwise
int Token_equalsToken(Token* first, Token* second);
// convert a token to a string
char* Token_toString(void* theToken);

#endif
