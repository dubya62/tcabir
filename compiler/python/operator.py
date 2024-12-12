
from debug import *
from token import *

class Operator:
    def __init__(self, tokens:list[Token], varnum:int):
        self.tokens = tokens
        self.varnum = varnum

        # combine multi-token operations
        self.tokens = self.combine_multi_token_operations(self.tokens)

        # break operations
        self.tokens = self.break_operations(self.tokens)

        
    def combine_multi_token_operations(self, tokens:list[Token]) -> list[Token]:

        first_tokens = set(["+", "/", "*", "!", "=", "%", "^", "~", "-"])
        second_tokens = set(["<", ">", "&", "|"])

        i = 0
        n = len(tokens)
        while i < n:
            if tokens[i].token in first_tokens:
                if i + 1 < n and tokens[i+1] == "=":
                    tokens[i].token += tokens[i+1].token
                    del tokens[i+1]
                    n -= 1
                    i += 1
                    continue
                if tokens[i] == "-":
                    if i + 1 < n and tokens[i+1] == ">":
                        tokens[i].token += tokens[i+1].token
                        del tokens[i+1]
                        n -= 1
                        i += 1
                        continue
            elif tokens[i].token in second_tokens:
                if i + 1 < n:
                    if tokens[i+1] == "=":
                        tokens[i].token += tokens[i+1].token
                        del tokens[i+1]
                        n -= 1
                        i += 1
                        continue
                    if tokens[i+1].token == tokens[i].token:
                        if i + 2 < n and tokens[i+2] == "=":
                            tokens[i].token = tokens[i+1].token + tokens[i+2].token
                            del tokens[i+1]
                            del tokens[i+1]
                            n -= 2
                            i += 1
                            continue
            i += 1


        return tokens


    def break_operations(self, tokens:list[Token]):
        dbg("Breaking operations from if statements")

        tokens = self.break_operations_from_ifs(tokens)

        tokens = self.break_operations_from_returns(tokens)

        tokens = self.convert_prefix_and_postfix(tokens)

        tokens = self.break_operations_from_function_calls(tokens)
        
        return tokens


    def break_operations_from_ifs(self, tokens:list[Token]) -> list[Token]:
        i = 0
        n = len(tokens)

        while i < n:
            if tokens[i] == "if":
                between = []
                parens = []
                starting_index = i
                while i < n:
                    if tokens[i] == "(":
                        break
                    i += 1
                while i < n:
                    if tokens[i] == "(":
                        parens.append("(")
                        i += 1
                        continue
                    elif tokens[i] == ")":
                        if len(parens) == 0:
                            fatal_error(tokens[i], "Mismatched )...")
                        parens.pop()
                        if len(parens) == 0:
                            break
                    between.append(tokens[i])
                    del tokens[i]
                    n -= 1
                tokens.insert(i, Token("#" + str(self.varnum), tokens[i].line_number, tokens[i].filename))
                i += 1
                n += 1
                tokens.insert(starting_index, Token("#" + str(self.varnum), tokens[starting_index].line_number, tokens[starting_index].filename))
                starting_index += 1
                i += 1
                n += 1
                tokens.insert(starting_index, Token("=", tokens[starting_index].line_number, tokens[starting_index].filename))
                starting_index += 1
                i += 1
                n += 1

                for x in between:
                    tokens.insert(starting_index, x)
                    starting_index += 1
                    i += 1
                    n += 1
                tokens.insert(starting_index, Token(";", tokens[starting_index].line_number, tokens[starting_index].filename))
                starting_index += 1
                i += 1
                n += 1

                self.varnum += 1
                
            i += 1

        return tokens


    def break_operations_from_returns(self, tokens:list[Token]) -> list[Token]:
        i = 0
        n = len(tokens)
        while i < n:
            if tokens[i] == "return":
                starting_index = i;
                after = []
                i += 1
                while i < n:
                    if tokens[i] == ";":
                        break
                    after.append(tokens[i])
                    del tokens[i]
                    n -= 1
                tokens.insert(i, Token("#" + str(self.varnum), tokens[i].line_number, tokens[i].filename))
                i += 1
                n += 1

                tokens.insert(starting_index, Token("#" + str(self.varnum), tokens[starting_index].line_number, tokens[starting_index].filename))
                i += 1
                n += 1
                starting_index += 1

                tokens.insert(starting_index, Token("=", tokens[starting_index].line_number, tokens[starting_index].filename))
                i += 1
                n += 1
                starting_index += 1

                for x in after:
                    tokens.insert(starting_index, x)
                    i += 1
                    n += 1
                    starting_index += 1
                tokens.insert(starting_index, Token(";", tokens[starting_index].line_number, tokens[starting_index].filename))
                i += 1
                n += 1
                starting_index += 1

                self.varnum += 1

            i += 1

        return tokens

    
    def convert_prefix_and_postfix(self, tokens:list[Token]) -> list[Token]:
        dbg("Converting prefix and postfix operators...")
        i = 0
        n = len(tokens)

        operators = set(["~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "=", ";", "++", "--"])
        
        while i < n:
            if tokens[i].token in ["++", "--"]:
                # figure out if this is prefix or postfix
                is_prefix = 0
                if i > 0 and (tokens[i-1].token in ["}", "{", ";", "("] or tokens[i-1].token in operators):
                    is_prefix = 1

                inside = []

                del tokens[i]
                n -= 1

                if is_prefix:
                    parens = []
                    j = i + 1
                    # higher:
                    # function call ()
                    # array subscript []
                    # .
                    # ->
                    # TODO: finish this
                    while j < n:
                        if tokens[j].token in ["(", "["]:
                            parens.append(tokens[j].token)
                        elif tokens[j] == ")":
                            pass
                        elif tokens[j] == "]":
                            pass

                        j += 1
                    # move to the line before
                    
                    pass
                else:
                    j = i - 1
                    parens = []
                    while j >= 0:
                        if tokens[j] == ")":
                            parens.append(")")
                        elif tokens[j] == "(":
                            if len(parens) == 0:
                                fatal_error(tokens[j], "Mismatched (...")
                            parens.pop()
                            if len(parens) == 0:
                                break
                        inside.append(tokens[j])

                        if len(parens) == 0:
                            break

                        j -= 1

                    # move to the line after
                    j = i
                    while j < n:
                        if tokens[j] == ";":
                            j += 1
                            break
                        j += 1

                    for x in inside:
                        tokens.insert(j, x)
                        j += 1; n += 1
                    tokens.insert(j, Token("+=", tokens[j].line_number, tokens[j].filename))
                    j += 1; n += 1
                    tokens.insert(j, Token("1", tokens[j].line_number, tokens[j].filename))
                    j += 1; n += 1
                    tokens.insert(j, Token(";", tokens[j].line_number, tokens[j].filename))
                    j += 1; n += 1
                        

            

            i += 1

        dbg("Finished converting prefix and postfix operators...")
        return tokens

    def break_operations_from_function_calls(self, tokens:list[Token]) -> list[Token]:
        i = 0
        n = len(tokens)
        changes = 1
        while changes:
            changes = 0
            while i < n:
                if len(tokens[i].token) > 0 and tokens[i].token[0] == '#':
                    # this is a variable
                    starting_index = i
                    if i + 1 < n and tokens[i+1] == "(":
                        # this is a function call or definition
                        # make sure the thing before the var is not $TYPE or $STRUCT x, $ENUM x, $UNION x
                        if not (i > 0 and tokens[i-1] == "$TYPE" or i > 1 and tokens[i-2] in ["$STRUCT", "$ENUM", "$UNION"]):
                            # this is a function call
                            while starting_index >= 0:
                                if tokens[starting_index].token in ["{", ";", "}"]:
                                    starting_index += 1
                                    break
                                starting_index -= 1
                            i += 1
                            braces = []
                            current = []
                            while i < n:
                                print(tokens[i])
                                if tokens[i].token in ["(", "{"]:
                                    braces.append(tokens[i].token)
                                    if len(braces) == 1:
                                        i += 1
                                        continue
                                elif tokens[i] == "}":
                                    if len(braces) == 0:
                                        fatal_error(tokens[i], "Mismatched }...")
                                    braces.pop()
                                elif tokens[i] == ")":
                                    if len(braces) == 0:
                                        fatal_error(tokens[i], "Mismatched )...")
                                    braces.pop()
                                    if len(braces) == 0:
                                        # handle what was left
                                        if len(current) > 0:
                                            if len(current) == 1 and len(current[0].token) > 0 and current[0].token[0] == "#":
                                                # this should just be put back
                                                tokens.insert(i, current[0])
                                                current = []
                                                i += 2
                                                n += 1
                                                break

                                            changes += 1

                                            tokens.insert(i, Token("#" + str(self.varnum), tokens[i].line_number, tokens[i].filename))
                                            i += 2
                                            n += 1

                                            tokens.insert(starting_index, Token("#" + str(self.varnum), tokens[starting_index].line_number, tokens[starting_index].filename))
                                            starting_index += 1
                                            i += 1
                                            n += 1
                                            tokens.insert(starting_index, Token("=", tokens[starting_index].line_number, tokens[starting_index].filename))
                                            starting_index += 1
                                            i += 1
                                            n += 1

                                            for x in current:
                                                tokens.insert(starting_index, x)
                                                starting_index += 1
                                                i += 1
                                                n += 1
                                            tokens.insert(starting_index, Token(";", tokens[starting_index].line_number, tokens[starting_index].filename))
                                            starting_index += 1
                                            i += 1
                                            n += 1
                                            current = []

                                            self.varnum += 1

                                        break
                                elif tokens[i] == ",":
                                    if len(current) == 1 and len(current[0].token) > 0 and current[0].token[0] == "#":
                                        # this should just be put back
                                        tokens.insert(i, current[0])
                                        i += 2
                                        n += 1
                                        current = []
                                        continue

                                    changes += 1

                                    tokens.insert(i, Token("#" + str(self.varnum), tokens[i].line_number, tokens[i].filename))
                                    i += 2
                                    n += 1
                                    self.varnum += 1

                                    tokens.insert(starting_index, Token("#" + str(self.varnum), tokens[starting_index].line_number, tokens[starting_index].filename))
                                    starting_index += 1
                                    i += 1
                                    n += 1
                                    tokens.insert(starting_index, Token("=", tokens[starting_index].line_number, tokens[starting_index].filename))
                                    starting_index += 1
                                    i += 1
                                    n += 1

                                    for x in current:
                                        tokens.insert(starting_index, x)
                                        starting_index += 1
                                        i += 1
                                        n += 1
                                    tokens.insert(starting_index, Token(";", tokens[starting_index].line_number, tokens[starting_index].filename))
                                    starting_index += 1
                                    i += 1
                                    n += 1
                                    current = []
                                    continue
                                    
                                current.append(tokens[i])
                                del tokens[i] 
                                n -= 1


                i += 1;

        return tokens




