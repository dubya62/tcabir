/**
 * The purpose of this file is to inline all functions and convert recursive functions into iterative versions
 */
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Stack;

public class Inliner{
    public ArrayList<String> tokens;
    public int finalVarnum;

    public Inliner(ArrayList<String> tokens, int finalVarnum){
        Main.debug("Starting the inliner...");
        this.tokens = tokens;
        this.finalVarnum = finalVarnum;

        // convert multitoken operations into single token operations
        this.tokens = combineMultiTokenOperations(this.tokens);


        // break operations
        this.tokens = breakOperations(this.tokens);

        // convert returns
        this.tokens = convertReturns(this.tokens);

        // inline all functions
        //this.tokens = inlineFunctions(this.tokens);

        Main.debug("Inliner is finished!");
        Main.debug("Inliner output:");
        Main.debug(this.tokens.toString());
    }

    private ArrayList<String> combineMultiTokenOperations(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // multi-token operators
        // +=
        // /=
        // *=
        // <=
        // >=
        // !=
        // ==
        // %=
        // ^=
        // &=
        // |=
        // ~=
        //
        // -=
        // ->
        // &&
        // ||
        //
        // &&=
        // ||=
        //
        // <<
        // >>
        // <<=
        // >>=
        
        for (int i=0; i<tokens.size(); i++){
            switch (tokens.get(i)){
                case "+":
                case "/":
                case "*":
                case "!":
                case "=":
                case "%":
                case "^":
                case "~":
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("=")){
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                    }
                    result.add(tokens.get(i));
                    break;
                case "-":
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("=") || tokens.get(i+1).equals(">")){
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                    }
                    result.add(tokens.get(i));
                    break;
                case "<":
                case ">":
                case "&":
                case "|":
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("=")){
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                        if (tokens.get(i+1).equals(tokens.get(i))){
                            if (i+2 < tokens.size() && tokens.get(i+2).equals("=")){
                                result.add(tokens.get(i) + tokens.get(i+1) + tokens.get(i+2));
                                i+=2;
                                continue;
                            }
                            result.add(tokens.get(i) + tokens.get(i+1));
                            i++;
                            continue;
                        }
                    }
                    result.add(tokens.get(i));
                    break;

                default:
                    result.add(tokens.get(i));
                    break;
            }

        }

        result = convertPrefixAndPostfix(result);

        return result;
    }


    private ArrayList<String> convertPrefixAndPostfix(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // combine the tokens
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("-") || tokens.get(i).equals("+")){
                if (i+1 < tokens.size()){
                    if (tokens.get(i).equals(tokens.get(i+1))){
                        result.add(tokens.get(i) + tokens.get(i+1));
                        i++;
                        continue;
                    }
                }
            }
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> breakOperations(ArrayList<String> tokens){
        // break stuff from if statements
        ArrayList<String> result = breakOperationsFromIfs(tokens);

        // break stuff out of return statements
        result = breakOperationsFromReturns(result);
        
        // break stuff out of function calls
        result = breakOperationsFromFunctionCalls(result);

        // convert unary operators to binary
        result = convertUnaryOperators(result);
        result = convertCallsAndAccesses(result);

        // convert assigment operations to assignment and the operation
        result = convertAssignmentOperations(result);

        // convert -> to *.
        result = convertArrowOperator(result);

        // break up lines that have more than one operation on them
        result = breakMultipleOperations(result);

        // There is now at most 2 operations per line (1 if there is no equal sign

        // remove prefix and postfix operators
        result = removePrefixAndPostfix(result);

        // remove expression delimitters
        result = removeExpressionDelimitters(result);

        /*
         REMAINING OPERATIONS:
        {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access"};
        */

        /*
        remove <= and => and != using !(>), !(<), and !(==)
        {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access"};
        */
        result = removeOrEqualOperators(result);
        result = breakMultipleOperations(result);
        result = removeExpressionDelimitters(result);

        /*
        remove || using  (a||b) = !(!a && !b)
        {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "==", "&&", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access"};
        */
        result = removeLogicalOr(result);
        result = breakMultipleOperations(result);
        result = removeExpressionDelimitters(result);

        // make sure all if statements have an else clause
        result = createElseClauses(result);

        /*
        remove && using nested if statments
        {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "==", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access"};
        */
        result = removeLogicalAnd(result);
        result = createElseClauses(result);

        /*
        remove lognot using if/else statments
        {"%", "^", "&", "<<", ">>", "|", "-", "+", "<", ">", "/", "*", "==", ".", "=", "deref", "ref", "bitnot", "call", "access"};
        */
        result = removeLognot(result);
        result = createElseClauses(result);
         
        /*
        convert derefs to accesses
        {"%", "^", "&", "<<", ">>", "|", "-", "+", "<", ">", "/", "*", "==", ".", "=", "ref", "bitnot", "call", "access"};
         
         */
        return result;
    }


    private ArrayList<String> removeLognot(ArrayList<String> tokens){
        // a = 0 lognot #1
        // =>
        // a = 1;
        // if (#1){
        //    a = 0;
        // }

        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("lognot")){
                int returnIndex = i;

                ArrayList<String> before = new ArrayList<>();
                i--;
                i--;
                boolean found = false;
                while (i > 0){
                    switch(tokens.get(i)){
                        case "{":
                        case "}":
                        case ";":
                        case ":":
                            found = true;
                            break;
                    }
                    if (found){
                        break;
                    }
                    if (!tokens.get(i).equals("def")){
                        before.add(0, tokens.get(i));
                    }
                    i--;
                }

                i = returnIndex;
                tokens.set(i-1, "1");
                tokens.set(i, ";");
                i++;
                tokens.add(i, "if");
                i++;
                tokens.add(i, "(");
                // look for the end of the line
                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    i++;
                }
                tokens.add(i, ")");
                i++;
                tokens.add(i, "{");
                i++;
                for (int j=0; j<before.size(); j++){
                    tokens.add(i, before.get(j));
                    i++;
                }
                tokens.add(i, "0");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "}");

            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> removeLogicalAnd(ArrayList<String> tokens){
        // remove && using nested if statments
        ArrayList<String> result = new ArrayList<>();


        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("&&")){
                // initialize to 0 then nest two if statements to set to 1
                // look for = or ; or { or } or :
                int startingIndex = i;

                boolean found = false;
                ArrayList<String> before = new ArrayList<>();
                boolean gettingBefore = false;

                int beginningIndex = 0;
                while (i > 0){
                    switch(tokens.get(i)){
                        case "=":
                            gettingBefore = true;
                            beginningIndex = i+1;
                            break;
                        case ";":
                        case ":":
                        case "{":
                        case "}":
                            found = true;
                            i++;
                            if (beginningIndex == 0){
                                beginningIndex = i;
                            }
                            break;
                    }
                    if (found){
                        break;
                    }
                    if (gettingBefore && !tokens.get(i).equals("=") && !tokens.get(i).equals("def")){
                        before.add(0, tokens.get(i));
                    }
                    i--;
                }
                i = beginningIndex;
                tokens.add(i, "0");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "if");
                i++;
                tokens.add(i, "(");
                i++;

                i = startingIndex + 4;
                tokens.add(i, ")");
                i++;
                tokens.add(i, "{");
                i++;
                tokens.set(i, "if");
                i++;
                tokens.add(i, "(");
                i++;
                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    i++;
                }
                tokens.add(i, ")");
                i++;
                tokens.add(i, "{");
                i++;
                for (int j=0; j<before.size(); j++){
                    tokens.add(i, before.get(j));
                    i++;
                }
                tokens.add(i, "=");
                i++;
                tokens.add(i, "1");
                i++;
                tokens.add(i, ";");
                i++;
                tokens.add(i, "}");
                i++;
                tokens.add(i, "}");
                i++;
                
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> createElseClauses(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("if")){
                int returnIndex = i;

                int openBraces = 0;

                while (i < tokens.size()){
                    if (tokens.get(i).equals("{")){
                        openBraces++;
                    } else if (tokens.get(i).equals("}")){
                        openBraces--;
                        if (openBraces == 0){
                            i++;
                            break;
                        }
                    }
                    i++;
                }
                if (i < tokens.size()){
                    if (tokens.get(i).equals("else")){
                    } else {
                        // put an else here
                        tokens.add(i, "else");
                        i++;
                        tokens.add(i, "{");
                        i++;
                        tokens.add(i, "}");
                    }
                }
                i = returnIndex;
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> removeExpressionDelimitters(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("StartExpression") || tokens.get(i).equals("EndExpression")){
                continue;
            }
            result.add(tokens.get(i));
        }
        return result;
    }


    private ArrayList<String> removeLogicalOr(ArrayList<String> tokens){
        // remove || using  (a||b) = !(!a && !b)
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("||")){
                tokens.set(i, "&&");
                int returnIndex = i;
                tokens.add(i, ")");
                i++;
                i++;
                tokens.add(i, "0");
                i++;
                tokens.add(i, "lognot");
                i++;
                tokens.add(i, "(");
                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        tokens.add(i, ")");
                        tokens.add(i, ")");
                        break;
                    }
                    i++;
                }


                i = returnIndex;
                boolean found = false;
                while (i > 0){
                    switch(tokens.get(i)){
                        case "=":
                        case "{":
                        case ";":
                        case "}":
                            found = true;
                            break;
                    }
                    if (found){
                        i++;
                        break;
                    }
                    i--;
                }
                tokens.add(i, "0");
                i++;
                tokens.add(i, "lognot");
                i++;
                tokens.add(i, "(");
                i++;
                tokens.add(i, "0");
                i++;
                tokens.add(i, "lognot");
                i++;
                tokens.add(i, "(");


            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> removeOrEqualOperators(ArrayList<String> tokens){
        // remove <= and >= and != using !(>), !(<), and !(==)
        ArrayList<String> result = new ArrayList<>();

        String replacement = "";

        for (int i=0; i<tokens.size(); i++){
            switch(tokens.get(i)){
                case "<=":
                case ">=":
                case "!=":

                    if (tokens.get(i).equals("<=")){
                        replacement = ">";
                    } else if (tokens.get(i).equals(">=")){
                        replacement = "<";
                    } else {
                        replacement = "==";
                    }

                    tokens.set(i, replacement);
                    int returnIndex = i;

                    // look backwards for = or ; or {
                    boolean found = false;
                    while (i > 0){
                        switch(tokens.get(i)){
                            case "=":
                            case ";":
                            case "{":
                            case "}":
                                found = true;
                                break;
                        }
                        if (found){
                            i++;
                            break;
                        }
                        i--;
                    }
                    tokens.add(i, "0");
                    i++;
                    tokens.add(i, "lognot");
                    i++;
                    tokens.add(i, "(");

                    i = returnIndex;
                    while (i < tokens.size()){
                        if (tokens.get(i).equals(";")){
                            break;
                        }
                        i++;
                    }
                    tokens.add(i, ")");
                    i++;

                    break;
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }

    
    private ArrayList<String> removePrefixAndPostfix(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--", ","};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        int direction = 0;
        boolean insideExpression = false;
        int expressionStart = 0;
        for (int i=0; i<tokens.size(); i++){
            direction = 0;
            switch(tokens.get(i)){
                case "StartExpression":
                    expressionStart = i;
                    insideExpression = true;
                    tokens.remove(i);
                    i--;
                    break;
                case "EndExpression":
                    insideExpression = false;
                    tokens.remove(i);
                    i--;
                    break;

                case "pre++":
                    direction = 1;
                case "pre--":
                    // if inside expression, perform operation before
                    if (insideExpression){
                        tokens.add(expressionStart, tokens.get(i+1));
                        i++;
                        expressionStart++;
                        tokens.add(expressionStart, "=");
                        i++;
                        expressionStart++;
                        tokens.add(expressionStart, tokens.get(i+1));
                        i++;
                        expressionStart++;
                        if (direction == 1){
                            tokens.add(expressionStart, "+");
                        } else {
                            tokens.add(expressionStart, "-");
                        }
                        expressionStart++;
                        i++;
                        tokens.add(expressionStart, "1");
                        expressionStart++;
                        i++;
                        tokens.add(expressionStart, ";");
                        expressionStart++;
                        i++;
                        tokens.remove(i-1);
                        tokens.remove(i-1);
                        i++;
                        continue;
                    } else {
                        // otherwise, perform operation before this line
                        int returnIndex = i;
                        while (i > 0){
                            if (tokens.get(i).equals("{") || tokens.get(i).equals(";")){
                                i++;
                                break;
                            }
                            i--;
                        }
                        tokens.add(i, tokens.get(returnIndex+1));
                        returnIndex++;
                        i++;
                        tokens.add(i, "=");
                        returnIndex++;
                        i++;
                        tokens.add(i, tokens.get(returnIndex+1));
                        returnIndex++;
                        i++;
                        if (direction == 1){
                            tokens.add(i, "+");
                        } else {
                            tokens.add(i, "-");
                        }
                        returnIndex++;
                        i++;
                        tokens.add(i, "1");
                        returnIndex++;
                        i++;
                        tokens.add(i, ";");
                        i++;

                        tokens.remove(returnIndex);
                        tokens.remove(returnIndex);

                        i = returnIndex;
                        continue;
                    }
                case "post++":
                    direction = 1;
                case "post--":
                    // increment on the next line
                    tokens.remove(i);
                    tokens.remove(i);
                    int returningIndex = i;
                    while (i < tokens.size()){
                        if (tokens.get(i).equals(";")){
                            i++;
                            break;
                        }
                        i++;
                    }
                    tokens.add(i, tokens.get(returningIndex-1));
                    i++;
                    tokens.add(i, "=");
                    i++;
                    tokens.add(i, tokens.get(returningIndex-1));
                    i++;
                    if (direction == 1){
                        tokens.add(i, "+");
                    } else {
                        tokens.add(i, "-");
                    }
                    i++;
                    tokens.add(i, "1");
                    i++;
                    tokens.add(i, ";");

                    i = returningIndex;

                    break;
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> convertAssignmentOperations(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", ">>", "<<", "<<=", ">>=", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));


        for (int i=0; i<tokens.size(); i++){
            switch(tokens.get(i)){
                case "+=":
                case "/=":
                case "*=":
                case "%=":
                case "^=":
                case "&=":
                case "|=":
                case "~=":
                case "-=":
                case "&&=":
                case "||=":
                case "<<=":
                case ">>=":
                    ArrayList<String> before = new ArrayList<>();
                    int returnIndex = i;
                    while (i > 0){
                        if (tokens.get(i).equals("{") || tokens.get(i).equals(";")){
                            i++;
                            break;
                        }
                        i--;
                    }
                    while (i < returnIndex){
                        before.add(tokens.get(i));
                        i++;
                    }


                    result.add("=");
                    for (int j=0; j<before.size(); j++){
                        if (!before.get(j).equals("def")){
                            result.add(before.get(j));
                        }
                    }

                    // add the operation
                    if (tokens.get(i).length() > 0){
                        result.add(tokens.get(i).substring(0, tokens.get(i).length()-1));
                    } else {
                        System.out.println("Something is seriously wrong here...");
                        System.exit(1);
                    }

                    result.add("(");
                    returnIndex = i;
                    while (i < tokens.size()){
                        if (tokens.get(i).equals(";")){
                            break;
                        }
                        i++;
                    }
                    tokens.add(i, ")");
                    i = returnIndex+1;
            }
            result.add(tokens.get(i));
        }

        return result;
    }

    private ArrayList<String> convertArrowOperator(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "->", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        String[] higherPrecedence = {"post++", "post--", "call", "access", "."};
        Set<String> higherPrecedenceSet = new HashSet<>();
        higherPrecedenceSet.addAll(Arrays.asList(higherPrecedence));

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("->")){
                // go backwards until reaching something with lower precedence
                
                // abc->def->ghi
                // (*(*abc).def).ghi
                
                int returnIndex = i;
                i--;
                while (i > 0){
                    if (!(higherPrecedenceSet.contains(tokens.get(i))) && !(tokens.get(i).length() > 0 && tokens.get(i).charAt(0) == '#')){
                        // this is where the (* needs to go
                        if (tokens.get(i).equals(")")){
                            // go backwards until reaching the matching (
                            int openParens = 0;
                            while (i > 0){
                                if (tokens.get(i).equals("(")){
                                    openParens--;
                                    if (openParens == 0){
                                        break;
                                    }
                                } else if (tokens.get(i).equals(")")){
                                    openParens++;
                                }

                                i--;
                            }
                            i--;
                            continue;
                        }
                        break;
                    }
                    i--;
                }
                i++;
                tokens.add(i, "deref");
                tokens.add(i, "0");
                tokens.add(i, "(");

                i = returnIndex + 3;

                tokens.add(i, ".");
                tokens.add(i, ")");
                i+=2;
                tokens.remove(i);
                continue;
            }
        }

        for (int i=0; i<tokens.size(); i++){
            result.add(tokens.get(i));
        }

        return result;
    }

    
    private ArrayList<String> convertUnaryOperators(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // unary operators:
        // * - 0 deref
        // & - 0 ref
        // ! - 0 lognot
        // ~ - 0 bitnot
        // - - 0 -
        String[] unaryOperators = {"*", "&", "!", "~", "-", "++", "--"};
        Set<String> unaryOperatorsSet = new HashSet<>();
        unaryOperatorsSet.addAll(Arrays.asList(unaryOperators));
        
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "=", ";", "++", "--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        for (int i=0; i<tokens.size(); i++){
            // find unary operators
            if (unaryOperatorsSet.contains(tokens.get(i))){
                // if the thing before this is an operator, this is unary
                if (tokens.get(i).equals("++") || tokens.get(i).equals("--")){
                    if (i > 0 && tokens.get(i-1).length() > 0 && tokens.get(i-1).charAt(0) == '#'){
                        // postfix
                        result.add("post" + tokens.get(i));
                        result.add("0");
                    } else {
                        // prefix
                        result.add("0");
                        result.add("pre" + tokens.get(i));
                    }
                    continue;
                } else if (i > 0 && operatorsSet.contains(tokens.get(i-1))){
                    if (!tokens.get(i-1).equals("++") && !tokens.get(i-1).equals("--")){
                        result.add("0");
                    }
                    switch(tokens.get(i)){
                        case "*":
                            result.add("deref");
                            break;
                        case "&":
                            result.add("ref");
                            break;
                        case "!":
                            result.add("lognot");
                            break;
                        case "~":
                            result.add("bitnot");
                            break;
                        case "-":
                            result.add("-");
                            break;
                    }
                    continue;
                }
            }

            result.add(tokens.get(i));
        }

        return result;
    }

    private ArrayList<String> convertCallsAndAccesses(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "=", "deref", "ref", "lognot", "bitnot", "pre++", "pre--", "post++", "post--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        // () - call
        // [] - access

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("(") || tokens.get(i).equals("[")){
                // token before needs to be a var
                if (i > 0){
                    if ((tokens.get(i-1).length() > 0 && tokens.get(i-1).charAt(0) == '#') || tokens.get(i-1).equals(")")){
                        // cannot be a definition
                        if (i-1 > 0 && tokens.get(i-2).equals("def")){
                        } else {
                            switch(tokens.get(i)){
                                case "(":
                                    result.add("call");
                                    break;
                                case "[":
                                    result.add("access");
                                    // replace the [ with (
                                    tokens.set(i, "(");
                                    int returnIndex = i;
                                    int openBrackets = 1;
                                    while (i < tokens.size()){
                                        if (tokens.get(i).equals("[")){
                                            openBrackets++;
                                        } else if (tokens.get(i).equals("]")){
                                            openBrackets--;
                                            if (openBrackets == 0){
                                                tokens.set(i, ")");
                                                break;
                                            }
                                        }
                                        i++;
                                    }
                                    i = returnIndex;
                                    break;
                            }


                        }

                    }
                }
            }

            result.add(tokens.get(i));
        }

        return result;
    }
    

    private ArrayList<String> breakOperationsFromIfs(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // make sure that there is at most 1 operation per line
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "="};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));
        
        // break stuff out of if statements
        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("if")){
                ArrayList<String> betweenParenthesis = new ArrayList<>();
                // get tokens between the parenthesis
                int startingIndex = i;
                if (i+1 < tokens.size() && tokens.get(i+1).equals("(")){
                    i += 2;
                    int openParens = 1;
                    while (i < tokens.size()){
                        if (tokens.get(i).equals("(")){
                            openParens++;
                        } else if (tokens.get(i).equals(")")){
                            openParens--;
                            if (openParens == 0){
                                break;
                            }
                        }
                        betweenParenthesis.add(tokens.get(i));
                        i++;
                    }
                }
                int endParenths = i;

                // see if there are more than one operator/function call
                int numOperators = 0;
                for (int j=0; j<betweenParenthesis.size(); j++){
                    if (operatorsSet.contains(betweenParenthesis.get(j))){
                        // this is an operator
                        numOperators++;
                    }
                }

                // we need to break this line out
                if (numOperators > 0){
                    result.add("def");
                    result.add("#" + this.finalVarnum);
                    result.add("=");
                    for (int j=0; j<betweenParenthesis.size(); j++){
                        result.add(betweenParenthesis.get(j));
                    }
                    result.add(";");
                    result.add("if");
                    result.add("(");
                    result.add("#" + this.finalVarnum);
                    this.finalVarnum++;
                    i = endParenths;
                } else {
                    result.add("if");
                    i = startingIndex;
                    continue;
                }

            }
            result.add(tokens.get(i));
        }


        return result;
    }


    private ArrayList<String> breakOperationsFromReturns(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // make sure that there is at most 1 operation per line
        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "="};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("return")){
                // get the tokens between return and ;
                ArrayList<String> betweenStatements = new ArrayList<>();
                i++;
                int startingIndex = i;
                while (i < tokens.size()){
                    if (tokens.get(i).equals(";")){
                        break;
                    }
                    betweenStatements.add(tokens.get(i));
                    i++;
                }
                int finalIndex = i;

                // if there are any operators, move to the line before
                int operatorCount = 0;
                for (int j=0; j<betweenStatements.size(); j++){
                    if (operatorsSet.contains(betweenStatements.get(j))){
                        operatorCount++;
                    }
                }

                if (operatorCount > 0){
                    result.add("def");
                    result.add("#" + this.finalVarnum);
                    result.add("=");
                    for (int j=0; j<betweenStatements.size(); j++){
                        result.add(betweenStatements.get(j));
                    }
                    result.add(";");
                    result.add("return");
                    result.add("#" + this.finalVarnum);
                    result.add(";");

                    this.finalVarnum++;
                    i = finalIndex+1;
                } else {
                    result.add("return");
                    i = startingIndex;
                }
            }
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> breakOperationsFromFunctionCalls(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"~", "!", "%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "+=", "/=", "*=", "<=", ">=", "!=", "==", "%=", "^=", "&=", "|=", "~=", "-=", "->", "&&", "||", "&&=", "||=", ".", "(", "[", ">>", "<<", "<<=", ">>=", "="};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        ArrayList<String> newArgs = new ArrayList<>();

        for (int i=0; i < tokens.size(); i++){
            // need (not def)(variable_name)(...)
            if (tokens.get(i).length() > 0){
                if (tokens.get(i).charAt(0) == '#'){
                    if (i > 0){
                        if (!tokens.get(i-1).equals("def")){
                            if (i+1 < tokens.size() && tokens.get(i+1).equals("(")){
                                int placePoint = i;
                                while (placePoint > 0){
                                    if (tokens.get(placePoint).equals(";") || tokens.get(placePoint).equals("{")){
                                        placePoint++;
                                        break;
                                    }
                                    placePoint--;
                                }

                                // stuff inside the parenthesis are the arguments
                                int startingIndex = i;
                                i++;
                                int openParens = 0;
                                ArrayList<String> currentArg = new ArrayList<>();
                                while (i < tokens.size()){
                                    if (tokens.get(i).equals("(")){
                                        openParens++;
                                        if (openParens == 1){
                                            i++;
                                            continue;
                                        }
                                    } else if ((tokens.get(i).equals(",") || tokens.get(i).equals(")")) && openParens == 1){
                                        // this breaks up args
                                        // if there are any operators, replace this arg with new token
                                        int numOperators = 0;
                                        for (int j=0; j<currentArg.size(); j++){
                                            if (operatorsSet.contains(currentArg.get(j))){
                                                numOperators++;
                                            }
                                        }
                                        
                                        if (numOperators > 0){
                                            currentArg.add(0, "=");
                                            currentArg.add(0, "#" + this.finalVarnum);
                                            currentArg.add(0, "def");
                                            newArgs.add("#" + this.finalVarnum);
                                            this.finalVarnum++;
                                            currentArg = breakOperationsFromFunctionCalls(currentArg);
                                            for (int j=0; j<currentArg.size(); j++){
                                                result.add(placePoint, currentArg.get(j));
                                                placePoint++;
                                            }
                                            result.add(placePoint, ";");
                                            placePoint++;
                                        } else {
                                            for (int j=0; j<currentArg.size(); j++){
                                                newArgs.add(currentArg.get(j));
                                            }
                                        }


                                        currentArg = new ArrayList<>();

                                        if (tokens.get(i).equals(")")){
                                            break;
                                        }
                                        i++;
                                        continue;
                                    } else if (tokens.get(i).equals(")")){
                                        openParens--;
                                        if (openParens == 0){
                                            break;
                                        }
                                    }

                                    currentArg.add(tokens.get(i));

                                    i++;
                                }
                                int finalIndex = i+1;
                                i = startingIndex;

                                result.add(tokens.get(i));
                                result.add("(");
                                for (int j=0; j < newArgs.size(); j++){
                                    if (j != 0){
                                        result.add(",");
                                    }
                                    result.add(newArgs.get(j));
                                }
                                result.add(")");
                                i = finalIndex;

                                if (finalIndex >= tokens.size()){
                                    break;
                                }
                            }
                        }
                    }
                }
            }


            result.add(tokens.get(i));
        }

        return result;
    }


    /**
     * Break up normal lines that have more than one operation using operator precedence
     */
    private ArrayList<String> breakMultipleOperations(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--"};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));


        // iterate through the lines of the file and use infix to postfix to break up the lines
        ArrayList<String> currentLine;
        for (int i=0; i<tokens.size(); i++){
            // each line should end with a semicolon or a {
            int operatorCount = 0;
            boolean wasNoEqual = true;
            int startingIndex = i;
            currentLine = new ArrayList<>();
            while (i < tokens.size()){
                if (tokens.get(i).equals("=")){
                    wasNoEqual = false;
                } else if (tokens.get(i).equals("{") || tokens.get(i).equals(";")){
                    // this is the end of a line
                    break;
                }

                if (operatorsSet.contains(tokens.get(i))){
                    operatorCount++;
                }

                currentLine.add(tokens.get(i));
                i++;
            }

            if (operatorCount > 2 || (operatorCount > 1 && wasNoEqual)){
                // this line needs to be broken up
                ArrayList<String> brokenLine = breakLineWithPostfix(currentLine);
                result.add("StartExpression");
                for (int j=0; j<brokenLine.size(); j++){
                    result.add(brokenLine.get(j));
                }
                result.add("EndExpression");
            } else {
                for (int j=0; j<currentLine.size(); j++){
                    result.add(currentLine.get(j));
                }
            }

            if (i < tokens.size()){
                result.add(tokens.get(i));
            }
        }

        

        return result;
    }

    private ArrayList<String> breakLineWithPostfix(ArrayList<String> tokens){
        String[] operators = {"%", "^", "&", "|", "-", "+", "<", ">", "/", "*", "<=", ">=", "!=", "==", "&&", "||", ".", ">>", "<<", "=", "deref", "ref", "lognot", "bitnot", "call", "access", "pre++", "pre--", "post++", "post--", ","};
        Set<String> operatorsSet = new HashSet<>();
        operatorsSet.addAll(Arrays.asList(operators));

        // Create a HashMap with each operator's precedence for ordering
        HashMap<String, Integer> precedences = new HashMap();

        precedences.put("post++", 30);
        precedences.put("post--", 30);

        precedences.put("call", 29);
        precedences.put("access", 28);

        precedences.put(".", 27);

        precedences.put("pre++", 26);
        precedences.put("pre--", 26);

        precedences.put("lognot", 25);
        precedences.put("bitnot", 25);
        precedences.put("deref", 24);
        precedences.put("ref", 23);

        precedences.put("*", 22);
        precedences.put("%", 22);
        precedences.put("/", 22);

        precedences.put("+", 21);
        precedences.put("-", 21);

        precedences.put("<<", 20);
        precedences.put(">>", 20);

        precedences.put("<", 19);
        precedences.put("<=", 19);
        precedences.put(">", 18);
        precedences.put(">=", 18);

        precedences.put("==", 17);
        precedences.put("!=", 17);

        precedences.put("&", 16);

        precedences.put("^", 15);

        precedences.put("|", 14);

        precedences.put("&&", 13);

        precedences.put("||", 12);


        precedences.put(",", 11);


        precedences.put("=", 10);

        System.out.println("BREAKING LINE: " + tokens.toString());

        ArrayList<String> postfixExpression = new ArrayList<>();
        Stack<String> operatorStack = new Stack<>();

        // handle if the first token is def
        boolean addDef = false;
        if (tokens.size() > 0){
            if (tokens.get(0).equals("def")){
                addDef = true;
                tokens.remove(0);
            }
        }

        for (int i=0; i<tokens.size(); i++){
            if (operatorsSet.contains(tokens.get(i))){
                // if the precedence of the current operator is higher than the operator on the top of the stack, 
                // or stack is empty, or stack contains a (  => push
                if (operatorStack.isEmpty() || operatorStack.peek().equals("(") || precedences.get(operatorStack.peek()) < precedences.get(tokens.get(i))){
                    operatorStack.push(tokens.get(i));
                } else {
                    // pop all operators from the stack that have a precedence higher than or equal to current operator, then push current operator
                    while (true){
                        if (operatorStack.isEmpty()){
                            break;
                        }
                        if (operatorStack.peek().equals("(") || precedences.get(operatorStack.peek()) < precedences.get(tokens.get(i))){
                            break;
                        }
                        postfixExpression.add(operatorStack.pop());
                    }
                    operatorStack.push(tokens.get(i));
                }
                
            } else if (tokens.get(i).equals("(")){
                // if (, push to stack
                operatorStack.push("(");
            } else if (tokens.get(i).equals(")")){
                // pop from the stack until (
                while (true){
                    if (operatorStack.isEmpty()){
                        System.out.println("Mismatched parenthesis...");
                        System.exit(1);
                    }

                    if (operatorStack.peek().equals("(")){
                        operatorStack.pop();
                        break;
                    }

                    postfixExpression.add(operatorStack.pop());

                }
            } else {
                // if this is an operand, put it in the postfix expression
                postfixExpression.add(tokens.get(i));
            }
        }

        while (!operatorStack.isEmpty()){
            postfixExpression.add(operatorStack.pop());
        }

        System.out.println("Resulting expression: " + postfixExpression.toString());


        // TODO: now use the postfix expression to convert this into multiple lines of expressions, one on each line
        ArrayList<String> result = new ArrayList<>();

        Stack<String> operandStack = new Stack<>();
        for (int i=0; i<postfixExpression.size(); i++){
            if (operatorsSet.contains(postfixExpression.get(i))){
                // this is an operator
                if (operandStack.isEmpty()){
                    System.out.println("Operand Stack was empty during evaluation of expression...");
                    System.exit(1);
                }
                String second = operandStack.pop();
                if (operandStack.isEmpty()){
                    System.out.println("Operand Stack was empty during evaluation of expression...");
                    System.exit(1);
                }
                String first = operandStack.pop();

                // if the operator is not =, create a new variable and set it to this expression
                if (!postfixExpression.get(i).equals("=")){
                    result.add("def");
                    result.add("#" + this.finalVarnum);
                    result.add("=");
                    operandStack.push("#" + this.finalVarnum);
                    this.finalVarnum++;
                } else if (addDef){
                    result.add("def");
                }
                result.add(first);
                result.add(postfixExpression.get(i));
                result.add(second);
                result.add(";");

            } else {
                // this is an operand
                operandStack.push(postfixExpression.get(i));
            }
        }
        while (!operandStack.isEmpty()){
            result.add(operandStack.pop());
            result.add(";");
        }

        System.out.println("Postfix Evalution: " + result.toString());

        return result;
    }


    private ArrayList<String> convertReturns(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        // add an argument to each function that is a pointer to the return type

        for (int i=0; i<tokens.size(); i++){
            if (tokens.get(i).equals("def")){
                if (i+2 < tokens.size() && tokens.get(i+1).length() > 0 && tokens.get(i+1).charAt(0) == '#'){
                    if (tokens.get(i+2).equals("(")){
                        result.add(tokens.get(i));
                        result.add(tokens.get(i+1));
                        result.add(tokens.get(i+2));
                        result.add("def");
                        result.add("#" + this.finalVarnum);
                        // TODO: make type of this variable a pointer to the return type
                        if (i+3 < tokens.size() && !tokens.get(i+3).equals(")")){
                            result.add(",");
                        }
                        i += 2;
                        int returnIndex = i;

                        // TODO: look for all returns inside this function and replace
                        //      return x;
                        // WITH:
                        //      *returnVar = x
                        //      return;

                        this.finalVarnum++;
                        continue;
                    }
                }
            }
            result.add(tokens.get(i));
        }

        return result;
    }


    private ArrayList<String> inlineFunctions(ArrayList<String> tokens){
        ArrayList<String> result = new ArrayList<>();

        for (int i=0; i<tokens.size(); i++){
            // figure out if this token is a function definition
            int startingIndex = i;
            if (tokens.get(i).length() > 0){
                if (tokens.get(i).charAt(0) == '#'){
                    String functionName = tokens.get(i);
                    if (i+1 < tokens.size()){
                        if (tokens.get(i+1).equals("(")){
                            // tokens[i] is the function's name
                            // look for references to this function inside of its own scope
                            int openParens = 0;
                            while (i < tokens.size()){
                                if (tokens.get(i).equals("(")){
                                    openParens++;
                                } else if (tokens.get(i).equals(")")){
                                    openParens--;
                                    if (openParens == 0){
                                        break;
                                    }
                                }
                                i++;
                            }

                            i++;
                            if (i < tokens.size() && tokens.get(i).equals("{")){
                            } else {
                                i = startingIndex;
                                result.add(tokens.get(i));
                                continue;
                            }
                            
                            int openBraces = 0;
                            boolean isRecursive = false;
                            while (i < tokens.size()){
                                if (tokens.get(i).equals("{")){
                                    openBraces++;
                                } else if (tokens.get(i).equals("}")){
                                    openBraces--;
                                    if (openBraces == 0){
                                        break;
                                    }
                                } else if (tokens.get(i).equals(functionName)){
                                    // this is a recursive function
                                    isRecursive = true;
                                }
                                i++;
                            }


                        }
                    }
                }
            }
        }

        return result;
    }
}


