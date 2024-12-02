#ifndef TOKEN_H
#define TOKEN_H

typedef enum TokenType{
    VARIABLE,
    FUNCTION,
    BUILTIN_FUNCTION,
    OPERATOR,
    DELIMITER,
    LITERAL,
    UNKNOWN,
} TokenType;

typedef enum BuiltinFunctions{
    MALLOC,
    FREE,
    READ,
    WRITE,
} BuiltinFunctions;

typedef enum Operators{
    BITWISE_NOT,
    LOGICAL_NOT,
    HASHTAG,
    MOD,
    XOR,
    BITWISE_AND,
    TIMES,
    REFERENCE,
    DEREFERENCE,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    MINUS,
    PLUS,
    EQUALS,
    OPEN_BRACES,
    CLOSE_BRACES,
    OPEN_BRACKETS,
    CLOSE_BRACKETS,
    BITWISE_OR,
    SLASH,
    QUESTION_MARK,
    DOT,
    COMMA,
    LESS_THAN,
    GREATER_THAN,
    POSTFIX_INCREMENT,
    POSTFIX_DECREMENT,
    PREFIX_INCREMENT,
    PREFIX_DECREMENT,
    CALL,
    ACCESS,
    ARROW,
    CAST,
    UNARY_PLUS,
    UNARY_MINUS,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LESS_THAN_OR_EQUAL_TO,
    GREATER_THAN_OR_EQUAL_TO,
    EQUAL_EQUAL,
    NOT_EQUAL,
    PLUS_EQUALS,
    MINUS_EQUALS,
    TIMES_EQUALS,
    SLASH_EQUALS,
    MOD_EQUALS,
    LEFT_SHIFT_EQUALS,
    RIGHT_SHIFT_EQUALS,
    BITWISE_OR_EQUALS,
    BITWISE_AND_EQUALS,
    XOR_EQUALS,
    LOGICAL_OR_EQUALS,
    LOGICAL_AND_EQUALS,
    SIZEOF,
} Operators;

typedef enum Delimiters{
    SEMICOLON,
    COLON,
    SPACE,
    NEWLINE,
    TAB,
    BACKSLASH,
    APOSTROPHE,
    QUOTE,
} Delimiters;

typedef enum Literals{
    FLOAT,
    STRING,
    CHAR,
    INT,
} Literals;

typedef struct Token {
    int type;
    int value;
    int lineNumber;
    char* filename;
    char* token;
} Token;

// return the token representation of a string (parsing the type and value)
Token* stringToToken(char* theString);
// return 1 if the token is equivalent to a given string; 0 otherwise
int Token_equalsString(Token* first, char* theString);
// return 1 if the token is equivalent to another token; 0 otherwise
int Token_equalsToken(Token* first, Token* second);
// convert a token to a string
char* Token_toString(void* theToken);

#endif
