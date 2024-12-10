
from token import *
from debug import *

import types


class Simplifier:
    def __init__(self, tokens:list[Token]):
        self.tokens = tokens

        self.structures = []
        self.unions = []
        self.enums = []

        # convert type tokens
        self.tokens, _ = self.convert_type_tokens(self.tokens, 0, len(self.tokens))
        
        # handle typedefs

        # create extra scopes

        # generalize variables

        # fix function scoping

    
    def convert_type_tokens(self, tokens:list[Token], starting_index, ending_index) -> list[Token]:
        # convert all types to Type Tokens
        dbg("Converting Type Tokens...")
        i = starting_index
        n = len(tokens)
        if ending_index < n:
            n = ending_index
        starting_n = n

        builtin_types = set(["short", "long", "int", "float", "double", "void", "signed", "unsigned", "const", "char"])

        dbg(f"Converting types of: {tokens[i:n]}")


        while i < n:
            if tokens[i].token in ["struct", "union"]:
                # if a struct or union, handle everything inside first
                return_index = i
                brace_stack = []
                new_starting_index = i + 1
                new_ending_index = n

                while i < n:
                    if tokens[i] == "{":
                        break;
                    i += 1
                while i < n:
                    if tokens[i] == "{":
                        brace_stack.append("{")
                    elif tokens[i] == "}":
                        if len(brace_stack) == 0:
                            fatal_error(tokens[i], "Mismatched }...")
                        brace_stack.pop()
                        if len(brace_stack) == 0:
                            new_ending_index = i + 1
                            break;
                    i += 1

                # everything inside is now converted. Now create a new token for this struct

                tokens, n_difference = self.convert_type_tokens(tokens, new_starting_index, new_ending_index)
                n += n_difference

                # we now have everything inside the struct taken care of
                # create a $STRUCT or $UNION token

            elif tokens[i].token in builtin_types:
                n_difference = self.replace_type(tokens, i, builtin_types)
                n += n_difference

            
            i += 1

        dbg("Finished Converting Type Tokens!")
        for x in tokens:
            if x == "$TYPE":
                print(f"TYPE: {x.types} {x.line_number}")

        return tokens, n - starting_n
    

    def replace_type(self, tokens:list[Token], i:int, builtin_types:set):
        the_line_number = tokens[i].line_number
        the_filename = tokens[i].filename
        # eat up the tokens to get the correct type
        current_type = []
        return_index = i
        n = len(tokens)
        starting_len = n
        while i < n:
            if tokens[i].token not in builtin_types:
                if tokens[i].token in ["=", "{", ";", "(", ","]:
                    break;
                i += 1
                continue
            current_type.append(tokens[i])
            del tokens[i]
            n -= 1
        i = return_index
        tokens.insert(i, types.Type(current_type, the_line_number, the_filename))
        return n - starting_len


