
import os

from token import *
from debug import *

import lexer
import normalizer



class Preprocessor:
    def __init__(self, tokens:list[Token], include_dirs:list[str], filename:str):
        self.tokens = tokens
        self.filename = filename

        self.definitions = {} # definition name : list of tokens

        splitted = filename.split("/")
        if len(splitted) <= 1:
            base_filename = ""
        else:
            base_filename = "/".join(splitted[:-1]) + "/"
        self.tokens = self.handle_compiler_directives(self.tokens, include_dirs, set(), base_filename)


    def handle_compiler_directives(self, tokens:list[Token], include_dirs:list[str], included_already:set, base_filename) -> list[Token]:
        dbg("Handling compiler directives...")

        i = 0
        n = len(tokens)

        result = []

        # TODO: replace with full solution
        while i < n:
            # handle defined words
            if tokens[i].token in self.definitions:
                substitution = self.definitions[tokens[i].token]
                for x in substitution:
                    result.append(x)
                i += 1
                continue

            if tokens[i] == "#":
                i += 1

                directive_tokens = []
                while i < n and tokens[i].token != "#END_DIRECTIVE":
                    directive_tokens.append(tokens[i])
                    i += 1

                if directive_tokens[0] == "define":
                    if len(directive_tokens) == 1:
                        error(directive_tokens[0], "Expected a token after define...")
                    self.definitions[directive_tokens[1].token] = directive_tokens[2:]
                    dbg("Added definition!")
                elif directive_tokens[0] == "undef":
                    pass
                elif directive_tokens[0] == "include":
                    dbg("Including file")
                    new_included_already = included_already.copy()
                    included_file = self.handle_include(directive_tokens, include_dirs, new_included_already, base_filename)
                    for x in included_file:
                        result.append(x)
                elif directive_tokens[0] == "pragma":
                    pass
                elif directive_tokens[0] == "ifndef":
                    pass
                elif directive_tokens[0] == "ifdef":
                    pass
                elif directive_tokens[0] == "if":
                    pass
                elif directive_tokens[0] == "endif":
                    pass
                elif directive_tokens[0] == "else":
                    pass
                elif directive_tokens[0] == "error":
                    pass
                elif directive_tokens[0] == "__FILE__":
                    pass
                elif directive_tokens[0] == "__LINE__":
                    pass
                elif directive_tokens[0] == "__DATE__":
                    pass
                elif directive_tokens[0] == "__TIME__":
                    pass
                elif directive_tokens[0] == "__TIMESTAMP__":
                    pass
                else:
                    fatal_error(directive_tokens[0], "Unrecognized Preprocessor Directive...")

                i += 1
                continue

            result.append(tokens[i])

            i += 1

        dbg("Finished handling compiler directives.")
        return result


    def handle_include(self, tokens:list[Token], include_dirs:list[str], included_already:set, base_filename:str) -> list[Token]:
        print(tokens)

        dbg("Performing previous compilation steps on included code...")
        if len(tokens) < 2:
            fatal_error(tokens[0], "Expected a filename after include...")

        filename = ""
        if tokens[1].token == "<" and tokens[-1].token == ">":
            filename = tokens[2:-1]
            return self.handle_library_include(filename)
        elif len(tokens[1].token) > 2 and tokens[1].token[0] == '"' and tokens[-1].token[-1] == '"':
            filename = tokens[1].token[1:-1]

        filename = base_filename + filename

        # look for this file name in each include dir
        the_dir = None
        if os.path.isfile(filename):
            the_dir = "./"
        else:
            for x in include_dirs:
                if os.path.isfile(x + filename):
                    the_dir = x
                    break

        if the_dir == None:
            fatal_error(tokens[0], f"Local file {filename} does not exist...")

        filename = f"{the_dir}{filename}"
        while filename[:2] == "./":
            filename = filename[2:]
        dbg(f"Including local file: {filename}")

        if filename in included_already:
            fatal_error(tokens[0], f"This is a circular include...\n{included_already}")

        included_already.add(filename)
        result = lexer.Lexer(filename).tokens
        result = normalizer.Normalizer(result).tokens

        splitted = filename.split("/")
        if len(splitted) <= 1:
            new_base_filename = ""
        else:
            new_base_filename = "/".join(splitted[:-1]) + "/"

        result = self.handle_compiler_directives(result, include_dirs, included_already, new_base_filename)

        dbg("Finished Previous compilation steps on included code!")
        return result
        

    def handle_library_include(self, filename:str)->list[Token]:
        dbg(f"Including library: {filename}")
        return []

        
