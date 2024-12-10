
from token import *
from debug import *

class Simplifier:
    def __init__(self, tokens:list[Token]):
        self.tokens = tokens

        # convert type tokens
        self.tokens = self.convert_type_tokens(self.tokens)
        
        # handle typedefs

        # create extra scopes

        # generalize variables

        # fix function scoping

    
    def convert_type_tokens(self, tokens:list[Token]) -> list[Token]:
        return tokens
