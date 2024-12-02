
#include <stdlib.h>
#include <stdio.h>

#include "token.h"
#include "string.h"
#include "builtins.h"

struct ValueAndType{
    int value;
    int type;
};


int isIntegerLiteral(char* theString){
    int theLength = strlen(theString);
    if (theLength == 0){
        return 0;
    }
    // Determine if this only contains digits 0-9
    for (int i=0; i<theLength; i++){
        if ((int)(theString[i]) > 57 || (int)(theString[i]) < 48){
            return 0;
        }
    }

    return 1;
}

struct ValueAndType* getValueAndTypeOfString(char* theString){
    struct ValueAndType* result = malloc(sizeof(struct ValueAndType));

    int theLength = strlen(theString);

    // parse the string to figure out its type and value
    switch (theLength){
        case 0:
            result->type = UNKNOWN;
            result->value = 0;
            break;
        case 1: // this could be an operator
            result->type = OPERATOR;
            switch (theString[0]){
                case '~':
                    result->value = BITWISE_NOT;
                    break;
                case '!':
                    result->value = LOGICAL_NOT;
                    break;
                case '#':
                    result->value = HASHTAG;
                    break;
                case '%':
                    result->value = MOD;
                    break;
                case '^':
                    result->value = XOR;
                    break;
                case '&':
                    result->value = BITWISE_AND;
                    break;
                case '*':
                    result->value = TIMES;
                    break;
                case '(':
                    result->value = OPEN_PARENTHESIS;
                    break;
                case ')':
                    result->value = CLOSE_PARENTHESIS;
                    break;
                case '-':
                    result->value = MINUS;
                    break;
                case '+':
                    result->value = PLUS;
                    break;
                case '=':
                    result->value = EQUALS;
                    break;
                case '{':
                    result->value = OPEN_BRACES;
                    break;
                case '}':
                    result->value = CLOSE_BRACES;
                    break;
                case '[':
                    result->value = OPEN_BRACKETS;
                    break;
                case ']':
                    result->value = CLOSE_BRACKETS;
                    break;
                case '|':
                    result->value = BITWISE_OR;
                    break;
                case '\'':
                    result->type = DELIMITER;
                    result->value = APOSTROPHE;
                    break;
                case '"':
                    result->type = DELIMITER;
                    result->value = QUOTE;
                    break;
                case ';':
                    result->type = DELIMITER;
                    result->value = SEMICOLON;
                    break;
                case ':':
                    result->type = DELIMITER;
                    result->value = COLON;
                    break;
                case '/':
                    result->value = SLASH;
                    break;
                case '?':
                    result->value = QUESTION_MARK;
                    break;
                case '.':
                    result->value = DOT;
                    break;
                case ',':
                    result->value = COMMA;
                    break;
                case '<':
                    result->value = LESS_THAN;
                    break;
                case '>':
                    result->value = GREATER_THAN;
                    break;
                case '\n':
                    result->type = DELIMITER;
                    result->value = NEWLINE;
                    break;
                case '\t':
                    result->type = DELIMITER;
                    result->value = TAB;
                    break;
                case '\\':
                    result->type = DELIMITER;
                    result->value = BACKSLASH;
                    break;
                case ' ':
                    result->type = DELIMITER;
                    result->value = SPACE;
                    break;
                default:
                    if (isIntegerLiteral(theString)){
                        result->type = LITERAL;
                        result->value = INT;
                        break;
                    }
                    result->type = VARIABLE;
                    result->value = 0;
                    break;
            }
            break;
        default:
            // check if it matches any builtin function names
            if (HashMap_containsKey(BUILTIN_FUNCTIONS, theString)){
                result->type = BUILTIN_FUNCTION;
                result->value = *((int*) HashMap_get(BUILTIN_FUNCTIONS, theString));
                break;
            }

            // check if it is a delimiter
            if (HashMap_containsKey(DELIMITERS, theString)){
                result->type = DELIMITER;
                result->value = *((int*) HashMap_get(DELIMITERS, theString));
                break;
            }
            
            // check if it is an integer literal
            if (isIntegerLiteral(theString)){
                result->type = LITERAL;
                result->value = INT;
                break;
            }

            // check if it is a statement
            if (HashMap_containsKey(STATEMENTS, theString)){
                result->type = STATEMENT;
                result->value = *((int*) HashMap_get(STATEMENTS, theString));
                break;
            }

            // check if it is NULL
            if (!strcmp(theString, "NULL")){
                result->type = LITERAL;
                result->value = NULL_LITERAL;
                break;
            }

            // otherwise, just say it is a variable name (to change later if needed)
            result->type = VARIABLE;
            result->value = 0;

            break;
    }

    return result;
}


Token* stringToToken(char* theString){
    Token* result = (Token*) malloc(sizeof(Token));

    result->token = theString;

    struct ValueAndType* valueAndType = getValueAndTypeOfString(theString);

    result->type = valueAndType->type;
    result->value = valueAndType->value;

    return result;
}

// return 1 if the token is equivalent to a given string; 0 otherwise
int Token_equalsString(Token* first, char* theString){
    struct ValueAndType* second = getValueAndTypeOfString(theString);
    return first->type == second->type && first->value == second->value;
}

// return 1 if the token is equivalent to another token; 0 otherwise
int Token_equalsToken(Token* first, Token* second){
    return first->type == second->type && first->value == second->value;
}

char* Token_toString(void* theToken){
    return ((Token*) theToken)->token;
}

