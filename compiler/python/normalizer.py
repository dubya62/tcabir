
from debug import *
from token import *


class Normalizer:
    def __init__(self, tokens:list[Token]):
        self.tokens = tokens

        # remove comments and multi-line comments that are not in string
        self.tokens = self.remove_comments(self.tokens)
        
        # check compiler directive syntax
        self.tokens = self.check_directive_syntax(self.tokens)

        # combine strings into single tokens and remove whitespace

        # combine floats into single tokens


    def get_backslashes_before(self, tokens, index):
        result = 0
        for i in range(index-1, -1, -1):
            if tokens[i] == '\\':
                result += 1
            else:
                return result
        return result


        
    def remove_comments(self, tokens:list[Token]) -> list[Token]:
        dbg("Removing Comments From The Tokens...")

        result = []

        comment = False
        multi_line = False
        quotes = False
        char_quote = False

        i = 0
        n = len(tokens)
        while i < n:

            if tokens[i] == '/':
                if comment and multi_line:
                    if i and tokens[i-1] == "*":
                        comment = False
                        multi_line = False
                        i += 1
                        continue

                if i + 1 < n:
                    if not comment and not quotes:
                        if tokens[i+1] == '/':
                            comment = True
                            multi_line = False
                        elif tokens[i+1] == '*':
                            comment = True
                            multi_line = True

            elif tokens[i] == '"':
                if not quotes and not char_quote:
                    quotes = True
                elif quotes and not char_quote:
                    backslashes = self.get_backslashes_before(tokens, i)
                    if backslashes % 2 == 0:
                        quotes = False
            elif tokens[i] == "'":
                if not quotes and not char_quote:
                    char_quote = True
                elif not quotes and char_quote:
                    backslashes = self.get_backslashes_before(tokens, i)
                    if backslashes % 2 == 0:
                        char_quote = False

            # keeping track of newlines
            elif tokens[i] == '\n':
                if comment and not multi_line:
                    comment = False
                if quotes and i:
                    if tokens[i-1] == "\\":
                        fatal_error(tokens[i-1], "Syntax error. Unmatched \"")

            if not comment:
                result.append(tokens[i])

            i += 1

        dbg("Finished Removing Comments From The Tokens!")
        return result


    def check_directive_syntax(self, tokens:list[Token]) -> list[Token]:
        i = 0
        n = len(tokens)

        while i < n:
            if tokens[i] == "#END_DIRECTIVE":
                fatal_error(tokens[i], "Invalid token")
            elif tokens[i] == "#":
                # put #END_DIRECTIVE in place of the newline
                while i < n:
                    if tokens[i] == "\n":
                        tokens.insert(i, "#END_DIRECTIVE")
                        break;
                    i += 1
                break;

            i += 1

        return tokens
        
        
