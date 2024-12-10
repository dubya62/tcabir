
from debug import *
from token import *

import types

class Converter:
    def __init__(self, tokens:list[Token]) -> list[Token]:
        self.tokens = tokens
        
