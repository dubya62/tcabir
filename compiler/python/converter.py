
from debug import *
from token import *

import types

class Converter:
    def __init__(self, tokens:list[Token]) -> list[Token]:
        self.tokens = tokens

        self.current_label = 0

        # convert else if(){} into else { if(){} }
        self.tokens = self.convert_else_ifs(self.tokens)

        # convert breaks into jumps to the end of loops
        self.tokens = self.convert_breaks(self.tokens)

        # convert while loops to if statements and jumps

        # convert for loops to if statements and jumps


    def convert_else_ifs(self, tokens:list[Token]) -> list[Token]:
        i = 0
        n = len(tokens)
        while i < n:
            if tokens[i] == "else":
                if i + 1 < n:
                    if tokens[i+1] == "if":
                        tokens.insert(i+1, Token("{", tokens[i].line_number, tokens[i].filename))
                        i += 2
                        n += 1
                        braces = []
                        while i < n:
                            if tokens[i] == "{":
                                braces.append("{")
                            elif tokens[i] == "}":
                                if len(braces) == 0:
                                    fatal_error(tokens[i], "Mismatched }...")
                                braces.pop()
                                if len(braces) == 0:
                                    if i + 1 < n and tokens[i+1] == "else":
                                        i += 1
                                        continue
                                    tokens.insert(i, Token("}", tokens[i].line_number, tokens[i].filename))
                                    n += 1
                                    break
                            i += 1

            i += 1

        return tokens


    # TODO: handle nested loops with breaks in them
    def convert_breaks(self, tokens:list[Token]) -> list[Token]:
        i = 0
        n = len(tokens)

        brace_levels = []
        jump_labels = []
        open_braces = 0

        while i < n:
            print(tokens[i], i, n)
            if tokens[i] in ["for", "while", "switch"]:
                brace_levels.append(open_braces)
                jump_labels.append(self.current_label)
                self.current_label += 1
            elif tokens[i] == "{":
                open_braces += 1
            elif tokens[i] == "}":
                open_braces -= 1

                print("HERE", brace_levels, open_braces)
                if len(brace_levels) > 0 and brace_levels[-1] == open_braces:
                    brace_levels.pop()
                    i += 1
                    tokens.insert(i, Token("@" + str(jump_labels[-1]), tokens[i].line_number, tokens[i].filename))
                    i += 1
                    n += 1
                    tokens.insert(i, Token(":", tokens[i].line_number, tokens[i].filename))
                    i += 1
                    n += 1
                    self.current_label += 1
                    jump_labels.pop()
                    continue
            elif tokens[i] == "break":
                if len(jump_labels) == 0:
                    fatal_error(tokens[i], "Cannot break outside of loop...")
                del tokens[i]
                n -= 1
                tokens.insert(i, "goto")
                n += 1
                i += 1
                tokens.insert(i, "@" + str(jump_labels[-1]))
                n += 1
                i += 1
                continue

            i += 1

        return tokens
        


