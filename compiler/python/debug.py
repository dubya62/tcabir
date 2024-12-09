
from token import *

DEBUG = 1
ERRORS = []

def dbg(message:str):
    if DEBUG:
        print(message)

def panic(message:str):
    print(message)
    exit(1)

def error(token:Token, message:str):
    result = "Error: {token.filename}:{token.line_number} at token {token.token}\n\t{message}"
    ERROR.append(result)

def fatal_error(token:Token, message:str):
    [print(err) for err in ERRORS]

    print(f"Fatal Error: {token.filename}:{token.line_number} at token {token.token}")
    print(f"\t{message}")

    print("Encountered fatal error... exiting!")
    exit(1)



