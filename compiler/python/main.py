
import sys

from debug import *

import cli_parser
import lexer
import normalizer


class Main:
    def __init__(self):
        pass


    def start(self) -> int:
        dbg("Starting the TCABIR compiler...")

        # parse cli args
        dbg("##############################")
        dbg("Parsing Command Line Arguments")
        cli_args = cli_parser.CliArgs(sys.argv)
        
        # perform lexing
        dbg("##############################")
        dbg("Performing Lexical Analysis...")
        the_lexer = lexer.Lexer(cli_args.input_files[0])
        tokens = the_lexer.tokens

        dbg("Lexical Anaylsis Finished...")
        dbg("Resulting Tokens:")
        dbg(tokens)

        # basic normalization
        dbg("##############################")
        dbg("Performing Basic Normalization...")
        the_normalizer = normalizer.Normalizer(tokens)
        tokens = the_normalizer.tokens

        dbg("Basic Normalization Finished...")
        dbg("Resulting Tokens:")
        dbg(tokens)

        # compiler directives
        # simplification



        return 0


if __name__ == '__main__':
    main = Main()
    main.start()

        